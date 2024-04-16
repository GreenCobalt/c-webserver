#ifndef _DEFS_H
#define _DEFS_H

typedef enum
{
    GET,
    POST,
    HEAD,
    UNDEFINED
} REQUEST_TYPE;

const static struct
{
    REQUEST_TYPE request_type;
    const char *str;
} __REQUEST_TYPE_STRUCT[] = {
    {GET, "GET"},
    {POST, "POST"},
    {HEAD, "HEAD"},
};

REQUEST_TYPE string_to_request_type(const char *str);
const char* request_type_to_string(REQUEST_TYPE type);

typedef struct
{
    REQUEST_TYPE request_type;
    char *path;
    char *http_version;
} request_info;

void free_request_info(request_info info);

#endif /* _DEFS_H */