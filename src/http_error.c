#include "include/http_error.h"

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

response_info http_code_to_response(int code)
{
    for (int i = 0; i < (sizeof(__HTTP_ERROR_PAGES) / sizeof(__HTTP_ERROR_PAGES[0])); i++)
    {
        if (code == __HTTP_ERROR_PAGES[i].code)
        {
            return __HTTP_ERROR_PAGES[i].page;
        }
    }
    return (response_info){.content = {.content = "501 Not Implemented", .mime_type = "text/plain; charset=utf-8", .size = 19}, .content_length = 19, .http_code = 501};
}