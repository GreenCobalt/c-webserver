#include "include/strfunc.h"
#include "include/server.h"
#include "include/defs.h"
#include "include/file.h"

#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <arpa/inet.h>

const int MAX_REQ_SIZE = 32768;
const int MAX_HEADER_RESP_SIZE = 256;
const int port = 9000;

request_info parse_reqline(char *reqline)
{
    char *edit = calloc(strlen(reqline) + 1, sizeof(char));
    strncpy(edit, reqline, strlen(reqline));

    request_info result;

    result.request_type = string_to_request_type(strtok(edit, " "));
    result.path = strdup(strtok(NULL, " "));
    result.http_version = strdup(strtok(NULL, " "));

    if (strcmp(result.path, "/") == 0)
    {
        result.path = strdup("/index.html");
    }

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

        // get requesting socket ip
        struct sockaddr_in *pV4Addr = (struct sockaddr_in *)sock_addr;
        struct in_addr ipAddr = pV4Addr->sin_addr;
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, ip_str, INET_ADDRSTRLEN);

        // parse http request
        char *request_text = strreplace(strreplace(req, "\r\n", "\n", 1), "\n\n", "|", 1);
        char *REQLINE = strtok(request_text, "\n");
        char *HEADERS = strtok(NULL, "|");
        char *REQBODY = strtok(NULL, "|");
        if (REQBODY == NULL)
            REQBODY = "";

        // parse "GET / HTTP/1.0" line into request_info struct
        request_info request = parse_reqline(REQLINE);
        printf("%s\t%s\t%s\t\t-> ", ip_str, request_type_to_string(request.request_type), request.path);

        response_info response = {};

        /*
        // generate response html
        char *final_html = calloc(MAX_REQ_SIZE, sizeof(char));
        char *headers_replace = strreplace(HEADERS, "\n", "<br>", 0);
        snprintf(final_html, MAX_REQ_SIZE, "<html><body><b>IP:</b><br>%s<br><br><b>REQLINE (%p, %ld):</b><br>%s<br><br><b>HEADERS (%p, %ld):</b><br>%s<br><br><b>REQBODY (%p, %ld):</b><br>%s</body></html>", str, &REQLINE, strlen(REQLINE), REQLINE, &HEADERS, strlen(HEADERS), headers_replace, &REQBODY, strlen(REQBODY), REQBODY); //, &REQBODY, strlen(REQBODY), REQBODY);
        free(headers_replace);
        */

        char *path = calloc(8192, sizeof(char));
        snprintf(path, 8192, "%s%s", "webroot", request.path);
        response.content = read_file(path);
        response.content.mime_type = file_name_to_mime_type(path);
        if (response.content.size == 0)
        {
            response.http_code = 404;
            response.content.mime_type = strdup("text/plain");
            response.content.content = strdup("404 Not Found");
            response.content.size = strlen(response.content.content);
        }
        else
        {
            response.http_code = 200;
        }
        free(path);

        // generate date string for response header
        response.date = calloc(40, sizeof(char));
        time_t t = time(NULL);
        struct tm tm = *gmtime(&t);
        snprintf(response.date, 40, "%s, %02d %s %04d %02d:%02d:%02d %s", DAY[tm.tm_wday], tm.tm_mday, MONTH[tm.tm_mon], tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, "GMT");

        // generate response http packet
        int packet_response_size = MAX_HEADER_RESP_SIZE + strlen(response.date) + response.content.size;
        char *packet_response = calloc(packet_response_size, sizeof(char));
        snprintf(packet_response, packet_response_size, "HTTP/1.0 %d OK\r\nServer: snadol 0.1\r\nContent-Length: %ld\r\nContent-Type: %s\r\nDate: %s\r\n\r\n", response.http_code, response.content.size, response.content.mime_type, response.date);

        // adjust size of packet_response_size to match actual header size instead of max
        packet_response_size = strlen(packet_response) + response.content.size;

        // copy response contents to packet_response
        memcpy(&packet_response[strlen(packet_response)], response.content.content, response.content.size);

        // send html and close socket
        printf("%d\t%s\t%d\n", response.http_code, response.content.mime_type, packet_response_size);
        write(conn_socket, packet_response, packet_response_size);
        close(conn_socket);

        // free memory
        free(packet_response);
        free(request_text);
        free_request_info(&request);
        free_response_info(response);
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