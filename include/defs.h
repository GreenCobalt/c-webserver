#ifndef DEFS_H_
#define DEFS_H_

#include <string.h>
#include <stdlib.h>

typedef enum
{
    GET,
    POST,
    UNDEFINED
} REQUEST_TYPE;

const static struct
{
    REQUEST_TYPE request_type;
    const char *str;
} __REQUEST_TYPE_STRUCT[] = {
    {GET, "GET"},
    {POST, "POST"},
};

REQUEST_TYPE string_to_request_type(const char *str);
const char *request_type_to_string(REQUEST_TYPE type);

typedef struct
{
    REQUEST_TYPE request_type;
    char *path;
    char *http_version;
    int valid;
} request_info;

void free_request_info(request_info *info);

typedef struct
{
    char *content;
    const char *mime_type;
    long size;
} file_content;

typedef struct
{
    int http_code;
    file_content content;
    char *date;
} response_info;

void free_response_info(response_info info);

const static struct
{
    const int code;
    const char *message;
} __HTTP_CODE_MESSAGES[] = {
    {200, "OK"},
    {404, "Not Found"},
};

const char *http_code_to_message(int code);

typedef struct
{
    int conn_socket;
    struct sockaddr *sock_addr;
} connection_input;

#endif /* DEFS_H_ */