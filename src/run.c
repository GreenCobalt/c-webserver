#include "include/strfunc.h"
#include "include/server.h"
#include "include/defs.h"

#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <arpa/inet.h>

const int MAX_REQ_SIZE = 32768;
const int port = 9000;

request_info parse_reqline(char *reqline)
{
    char *edit = calloc(strlen(reqline) + 1, sizeof(char));
    strncpy(edit, reqline, strlen(reqline));

    request_info result;

    result.request_type = string_to_request_type(strtok(edit, " "));
    result.path = strdup(strtok(NULL, " "));
    result.http_version = strdup(strtok(NULL, " "));

    free(edit);
    return result;
}

void run(Server *server)
{
    const char *DAY[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char *MONTH[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    while (1)
    {
        // set variables and stuff
        char *req = calloc(MAX_REQ_SIZE, sizeof(char));
        struct sockaddr *sock_addr = (struct sockaddr *)&server->address;
        socklen_t address_length = (socklen_t)sizeof(server->address);

        // accept and read from socket
        int conn_socket = accept(server->socket, sock_addr, &address_length);
        read(conn_socket, req, 32768);

        // parse http request
        char *requested = strreplace(strreplace(req, "\r\n", "\n", 1), "\n\n", "|", 1);
        char *REQLINE = strtok(requested, "\n");
        char *HEADERS = strtok(NULL, "|");
        char *REQBODY = strtok(NULL, "|");
        if (REQBODY == NULL)
            REQBODY = "";

        // parse "GET / HTTP/1.0" line into request_info struct
        request_info reqinfo = parse_reqline(REQLINE);
        printf("%s %s\n", request_type_to_string(reqinfo.request_type), reqinfo.path);

        // get requesting socket ip
        struct sockaddr_in *pV4Addr = (struct sockaddr_in *)sock_addr;
        struct in_addr ipAddr = pV4Addr->sin_addr;
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);

        // generate response html
        char *final_html = calloc(MAX_REQ_SIZE, sizeof(char));
        char *headers_replace = strreplace(HEADERS, "\n", "<br>", 0);
        snprintf(final_html, MAX_REQ_SIZE, "<html><body><b>IP:</b><br>%s<br><br><b>REQLINE (%p, %ld):</b><br>%s<br><br><b>HEADERS (%p, %ld):</b><br>%s<br><br><b>REQBODY (%p, %ld):</b><br>%s</body></html>", str, &REQLINE, strlen(REQLINE), REQLINE, &HEADERS, strlen(HEADERS), headers_replace, &REQBODY, strlen(REQBODY), REQBODY); //, &REQBODY, strlen(REQBODY), REQBODY);
        free(headers_replace);

        // generate date string for response header
        char *current_date = calloc(40, sizeof(char));
        time_t t = time(NULL);
        struct tm tm = *gmtime(&t);
        snprintf(current_date, 40, "%s, %02d %s %04d %02d:%02d:%02d %s", DAY[tm.tm_wday], tm.tm_mday, MONTH[tm.tm_mon], tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, "GMT");

        // generate response http packet
        char *response = calloc(MAX_REQ_SIZE + 100, sizeof(char));
        if (reqinfo.request_type == HEAD)
            snprintf(response, 47 + strlen(current_date), "HTTP/1.0 200 OK\r\nServer: snadol 0.1\r\nDate:%s\r\n\r\n", current_date);
        else
            snprintf(response, 69 + strlen(current_date) + strlen(final_html), "HTTP/1.0 200 OK\r\nServer: snadol 0.1\r\nContent-Length: %ld\r\nDate: %s\r\n\r\n%s", strlen(final_html), current_date, final_html);

        // send html and close socket
        write(conn_socket, response, strlen(response));
        close(conn_socket);

        // free memory
        free(current_date);
        free(final_html);
        free(requested);
        free(response);
        free_request_info(reqinfo);
    }
}

int main()
{
    // create server struct with port
    Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port, 255, run);

    // run funciton as defined in struct
    printf("Listening on port %d\n", port);
    server.run(&server);
}