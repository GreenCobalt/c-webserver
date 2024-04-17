#include "include/strfunc.h"
#include "include/server.h"
#include "include/defs.h"
#include "include/file.h"

#include <confuse.h>

#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <arpa/inet.h>

const int MAX_REQ_SIZE = 32768;
const int MAX_HEADER_RESP_SIZE = 256;
cfg_bool_t debug = 0;

const char *DAY[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *MONTH[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

char *generate_date_string()
{
    char *res = calloc(40, sizeof(char));
    time_t t = time(NULL);
    struct tm tm = *gmtime(&t);
    snprintf(res, 40, "%s, %02d %s %04d %02d:%02d:%02d %s", DAY[tm.tm_wday], tm.tm_mday, MONTH[tm.tm_mon], tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, "GMT");
    return res;
}

response_info generate_response(char *path)
{
    response_info response = {};
    if (debug)
        printf("reading file\n");
    response.content = read_file(path);
    response.content.mime_type = file_name_to_mime_type(path);
    if (response.content.size == 0)
    {
        free(response.content.content);
        response.http_code = 404;
        response.content.mime_type = "text/plain";
        response.content.content = strdup("404 Not Found");
        response.content.size = strlen(response.content.content);
    }
    else
    {
        response.http_code = 200;
    }

    // generate date string for response header
    if (debug)
        printf("generating date\n");
    response.date = generate_date_string();

    return response;
}

request_info parse_reqline(char *reqline)
{
    if (!reqline)
    {
        request_info result;
        result.valid = 0;
        return result;
    }

    if (debug)
        printf("%s\n", reqline);

    char *edit = calloc(strlen(reqline) + 1, sizeof(char));
    if (debug)
        printf("%p\n", edit);
    strncpy(edit, reqline, strlen(reqline));

    request_info result;
    result.valid = 1;

    if (debug)
        printf("%s\n", edit);

    result.request_type = string_to_request_type(strtok(edit, " "));

    if (debug)
        printf("%d\n", result.request_type);

    char *path = strtok(NULL, " ");
    char *http_version = strtok(NULL, " ");

    if (debug)
        printf("%s\n", path);

    result.path = strdup(path);
    result.http_version = strdup(http_version);

    if (strcmp(result.path, "/") == 0)
    {
        free(result.path);
        result.path = strdup("/index.html");
    }

    if (debug)
        printf("%s\n", result.path);

    free(edit);
    return result;
}

char *req;
void run(Server *server)
{
    while (1)
    {
        // set variables and stuff
        struct sockaddr *sock_addr = (struct sockaddr *)&server->address;
        socklen_t address_length = (socklen_t)sizeof(server->address);

        // accept and read from socket
        int conn_socket = accept(server->socket, sock_addr, &address_length);
        req = calloc(MAX_REQ_SIZE, sizeof(char));

        clock_t start = clock();

        if (debug)
            printf("=============================================================\nreading from socket\n");
        read(conn_socket, req, 32768);

        if (!req)
        {
            printf("REQUEST NULL\n");
            close(conn_socket);
            continue;
        }

        // get requesting socket ip
        if (debug)
            printf("getting client IP\n");
        struct sockaddr_in *pV4Addr = (struct sockaddr_in *)sock_addr;
        struct in_addr ipAddr = pV4Addr->sin_addr;
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, ip_str, INET_ADDRSTRLEN);

        // parse http request
        if (debug)
            printf("parsing http request\n");

        char *request_text = strreplace(strreplace(req, "\r\n", "\n", 1), "\n\n", "|", 1); /* THIS FREES req */
        char *REQLINE = strtok(request_text, "\n"), *HEADERS = strtok(NULL, "|"), *REQBODY = strtok(NULL, "|");
        if (REQBODY == NULL)
            REQBODY = "";

        // parse "GET / HTTP/1.0" line into request_info struct
        if (debug)
            printf("parsing reqline\n");

        request_info request = parse_reqline(REQLINE);
        if (!request.valid)
        {
            printf("REQUEST CANCELLED\n");
            close(conn_socket);
            free(request_text);
            continue;
        }

        printf("%s\t%s\t\t%s\n", ip_str, request_type_to_string(request.request_type), request.path);

        if (debug)
            printf("generating response html\n");
        char *file_path = calloc(8192, sizeof(char));
        snprintf(file_path, 8192, "%s%s", "webroot", request.path);
        response_info response = generate_response(file_path);
        free(file_path);

        // generate response http packet
        if (debug)
            printf("generating response http\n");
        int packet_response_size = MAX_HEADER_RESP_SIZE + strlen(response.date) + response.content.size;
        char *packet_response = calloc(packet_response_size, sizeof(char));
        snprintf(packet_response, packet_response_size, "HTTP/1.0 %d %s\r\nServer: snadol 0.1\r\nContent-Length: %ld\r\nContent-Type: %s\r\nDate: %s\r\n\r\n", response.http_code, http_code_to_message(response.http_code), response.content.size, response.content.mime_type, response.date);

        // adjust size of packet_response_size to match actual header size instead of max
        packet_response_size = strlen(packet_response) + response.content.size;

        // copy response contents to packet_response
        memcpy(&packet_response[strlen(packet_response)], response.content.content, response.content.size);

        // send html and close socket
        if (debug)
            printf("sending response\n");

        clock_t stop = clock();
        double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        printf("%d\t\t%s\t%d\t\t(%fms)\n", response.http_code, response.content.mime_type, packet_response_size, elapsed);

        write(conn_socket, packet_response, packet_response_size);
        close(conn_socket);

        // free memory
        free(packet_response);
        free(request_text);
        free_request_info(&request);
        free_response_info(response);
    }
}

cfg_t *cfg;
void sigintHandle(int dummy)
{
    printf("\nCaught CTRL+C\n");

    // free remaining pointers
    cfg_free(cfg);

    exit(0);
}

int main()
{
    signal(SIGINT, sigintHandle);

    // initialize config
    cfg_opt_t opts[] =
        {
            CFG_INT("port", 9000, CFGF_NONE),
            CFG_BOOL("debug", cfg_false, CFGF_NONE),
            CFG_END(),
        };
    cfg = cfg_init(opts, CFGF_NONE);
    if (cfg_parse(cfg, "server.conf") == CFG_PARSE_ERROR)
        return 1;

    int port = cfg_getint(cfg, "port");
    debug = cfg_getbool(cfg, "debug");

    // create server struct with port
    Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port, 255, run);

    // run funciton as defined in struct
    printf("http://127.0.0.1:%d/\n", port);
    server.run(&server);
}