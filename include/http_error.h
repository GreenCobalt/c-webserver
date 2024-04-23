#ifndef HTTP_ERROR_H_
#define HTTP_ERROR_H_

#include "include/def.h"

const static struct
{
    int code;
    response_info page;
} __HTTP_ERROR_PAGES[] = {
    {
        404,
        {.content = {.content = "404 Not Found", .mime_type = "text/plain; charset=utf-8", .size = 13}, .content_length = 13, .http_code = 404},
    },
    {
        501,
        {.content = {.content = "501 Not Implemented", .mime_type = "text/plain; charset=utf-8", .size = 19}, .content_length = 19, .http_code = 501},
    },
};

response_info http_code_to_response(int code);

#endif /* HTTP_ERROR_H_ */