#include "include/defs.h"

#include <string.h>
#include <stdlib.h>

REQUEST_TYPE string_to_request_type(const char *str)
{
    for (int i = 0; i < (sizeof(__REQUEST_TYPE_STRUCT) / sizeof(__REQUEST_TYPE_STRUCT[0])); i++)
    {
        if (!strcmp(str, __REQUEST_TYPE_STRUCT[i].str))
        {
            return __REQUEST_TYPE_STRUCT[i].request_type;
        }
    }
    return UNDEFINED;
}

const char *request_type_to_string(REQUEST_TYPE type)
{
    for (int i = 0; i < (sizeof(__REQUEST_TYPE_STRUCT) / sizeof(__REQUEST_TYPE_STRUCT[0])); i++)
    {
        if (type == __REQUEST_TYPE_STRUCT[i].request_type)
        {
            return __REQUEST_TYPE_STRUCT[i].str;
        }
    }
    return "UNDEFINED";
}

void free_request_info(request_info info) {
    free(info.http_version);
    free(info.path);
}