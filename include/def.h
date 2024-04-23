#ifndef DEFS_H_
#define DEFS_H_

#include <string.h>
#include <stdlib.h>

// file contents

typedef struct
{
    char *content;
    const char *mime_type;
    long size;
    int exists;
} file_content;

// http request types & response codes

typedef enum
{
    GET,
    HEAD,
    UNDEFINED
} REQUEST_TYPE;

const static struct
{
    REQUEST_TYPE request_type;
    const char *str;
} __REQUEST_TYPE_STRUCT[] = {
    {GET, "GET"},
    {HEAD, "HEAD"},
};

const static struct
{
    const int code;
    const char *message;
} __HTTP_CODE_MESSAGES[] = {
    {200, "OK"},
    {404, "Not Found"},
    {501, "Not Implemented"},
};

// request and response info

typedef struct
{
    int http_code;
    file_content content;
    int content_length;
    char *date;
} response_info;

typedef struct
{
    REQUEST_TYPE request_type;
    char *path;
    char *http_version;
    int valid;
} request_info;

// other

typedef struct
{
    int conn_socket;
    struct sockaddr *sock_addr;
} connection_input;

// FUNCTIONS

REQUEST_TYPE string_to_request_type(const char *str);
const char *request_type_to_string(REQUEST_TYPE type);
const char *http_code_to_message(int code);

#endif /* DEFS_H_ */