#include "include/date.h"
#include "include/file.h"
#include "include/http_def.h"
#include "include/http_error.h"
#include "include/server.h"
#include "include/str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include <confuse.h>
#include <magic.h>

#define DEBUG 0
#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%03d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

int port;
int max_request_content_size;
int max_response_header_size;

cfg_t *cfg;
magic_t magic;
int semaphore = 0;
int served = 0;

int read_from_socket(int socket, char *buffer, int max_size)
{
    if (read(socket, buffer, max_size) < 0)
    {
        printf("READ FROM SOCK FAILED\n");
        return 0;
    }

    if (!buffer)
    {
        printf("REQUEST NULL\n");
        return 0;
    }

    return 1;
}

void get_ip_from_socket(struct sockaddr *sock_addr, char *ip_str)
{
    struct sockaddr_in *pV4Addr = (struct sockaddr_in *)sock_addr;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    inet_ntop(AF_INET, &ipAddr, ip_str, INET_ADDRSTRLEN);
}

request_info parse_reqline(char *reqline)
{
    if (!reqline)
    {
        request_info result;
        result.valid = 0;
        return result;
    }

    DEBUG_PRINT("%s\n", reqline);

    char *edit = calloc(strlen(reqline) + 1, sizeof(char));
    DEBUG_PRINT("%p\n", edit);
    strncpy(edit, reqline, strlen(reqline));

    request_info result;
    result.valid = 1;

    result.request_type = string_to_request_type(strtok(edit, " "));

    char *path = strtok(NULL, " ");
    char *http_version = strtok(NULL, " ");

    result.path = strdup(path);
    result.http_version = strdup(http_version);

    if (strcmp(result.path, "/") == 0)
    {
        free(result.path);
        result.path = strdup("/index.html");
    }

    free(edit);
    return result;
}

response_info generate_response(request_info request, char *file_path)
{
    response_info response = {};

    if (request.request_type == UNDEFINED)
    {
        DEBUG_PRINT("returning 501\n");
        response = http_code_to_response(501);
    }
    else
    {
        if (file_exists(file_path))
        {
            DEBUG_PRINT("returning 200, reading file\n");
            response.content = read_file(file_path, magic);

            response.http_code = 200;
            response.content_length = response.content.size;
            if (request.request_type == HEAD)
                response.content.size = 0;
        }
        else
        {
            DEBUG_PRINT("returning 404\n");
            response = http_code_to_response(404);
        }
    }

    DEBUG_PRINT("generating date\n");
    response.date = generate_date_string();

    return response;
}

void free_request_info(request_info info)
{
    free(info.http_version);
    free(info.path);
}

void free_response_info(response_info info)
{
    free(info.date);
    if (info.http_code == 200)
        free(info.content.content);
}

void *handle_connection(void *vargp)
{
    clock_t start = clock();

    connection_input input = *((connection_input *)vargp);
    int conn_socket = input.conn_socket;
    struct sockaddr *sock_addr = input.sock_addr;

    DEBUG_PRINT("reading from socket\n");
    char *req = calloc(max_request_content_size, sizeof(char));
    if (!read_from_socket(conn_socket, req, max_request_content_size))
    {
        close(conn_socket);
        semaphore -= 1;
        return 0;
    }

    DEBUG_PRINT("getting client IP\n");
    char ip_str[INET_ADDRSTRLEN];
    get_ip_from_socket(sock_addr, ip_str);

    DEBUG_PRINT("parsing http request\n");
    char *request_text = strreplace(strreplace(req, "\r\n", "\n", 0), "\n\n", "|", 1);
    free(req);

    DEBUG_PRINT("splitting http request\n");
    char *REQLINE = strtok(request_text, "\n");
    /*char *HEADERS = */ strtok(NULL, "|");
    char *REQBODY = strtok(NULL, "|");

    if (REQBODY == NULL)
    {
        printf("reqbody is null\n");
        REQBODY = "";
    }

    DEBUG_PRINT("parsing reqline\n");
    request_info request = parse_reqline(REQLINE);
    if (!request.valid)
    {
        printf("REQUEST CANCELLED\n");
        close(conn_socket);
        free(request_text);
        semaphore -= 1;
        return 0;
    }

    printf("%s\t%s\t\t%s\n", ip_str, request_type_to_string(request.request_type), request.path);

    char *file_path = calloc(8192, sizeof(char));
    snprintf(file_path, 8192, "%s%s", "webroot", request.path);
    response_info response = generate_response(request, file_path);
    free(file_path);

    // generate response http packet
    DEBUG_PRINT("generating response http\n");
    int packet_response_size = max_response_header_size + strlen(response.date) + response.content.size;

    char *packet_response = calloc(packet_response_size, sizeof(char));
    snprintf(packet_response, packet_response_size, "HTTP/1.0 %d %s\r\nServer: snadol 0.1\r\nContent-Length: %d\r\nContent-Type: %s\r\nDate: %s\r\n\r\n", response.http_code, http_code_to_message(response.http_code), response.content_length, response.content.mime_type, response.date);

    // adjust size of packet_response_size to match actual header size instead of max
    packet_response_size = strlen(packet_response) + response.content.size;

    // copy response contents to packet_response
    if (request.request_type != HEAD)
        memcpy(&packet_response[strlen(packet_response)], response.content.content, response.content.size);

    // send html and close socket
    DEBUG_PRINT("sending response\n");

    clock_t stop = clock();
    double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("%d\t\t%s\t%d\t\t(%fms)\n", response.http_code, response.content.mime_type, packet_response_size, elapsed);

    packet_response[packet_response_size] = 'g';
    if (write(conn_socket, packet_response, packet_response_size) < 0)
    {
        printf("Error while writing\n");
    }
    close(conn_socket);

    // free memory
    free(packet_response);
    free(request_text);
    free_request_info(request);
    free_response_info(response);

    DEBUG_PRINT("freed pointers\n");

    DEBUG_PRINT("exiting\n");

    semaphore -= 1;
    return 0;
}

void run(Server *server)
{
    while (1)
    {
        // set variables and stuff
        struct sockaddr *sock_addr = (struct sockaddr *)&server->address;
        socklen_t address_length = (socklen_t)sizeof(server->address);

        // accept and read from socket
        int conn_socket = accept(server->socket, sock_addr, &address_length);
        if (conn_socket < 0)
            printf("Error on accept\n");

        // dispatch function to parse and repsond to request (TODO: MULTITHREADING)
        connection_input input = {
            .conn_socket = conn_socket,
            .sock_addr = sock_addr,
        };
        handle_connection((void *)&input);

        served += 1;
        semaphore += 1;
    }
}

void sigintHandle()
{
    printf("\nCaught CTRL+C\n");

    // free remaining pointers
    cfg_free(cfg);
    magic_close(magic);

    printf("Exiting, served %d requests total\n", served);
    exit(0);
}

int main()
{
    signal(SIGINT, sigintHandle);

    // initialize config and magic libraries
    cfg_opt_t opts[] =
        {
            CFG_INT("port", 9000, CFGF_NONE),
            CFG_INT("max_request_content_size", 32768, CFGF_NONE),
            CFG_INT("max_response_header_size", 512, CFGF_NONE),
            CFG_END(),
        };
    cfg = cfg_init(opts, CFGF_NONE);
    magic = magic_open(MAGIC_MIME);

    // load config and magic data
    if (cfg_parse(cfg, "server.conf") == CFG_PARSE_ERROR)
        return 1;
    magic_load(magic, NULL);

    port = cfg_getint(cfg, "port");
    max_request_content_size = cfg_getint(cfg, "max_request_content_size");
    max_response_header_size = cfg_getint(cfg, "max_response_header_size");

    // create server struct with port
    Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port, 255, run);

    // run function as defined in struct
    printf("http://127.0.0.1:%d/\n", port);
    server.run(&server);
}