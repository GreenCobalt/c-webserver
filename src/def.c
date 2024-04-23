#include "include/def.h"
#include "include/str.h"

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

const char *http_code_to_message(int code)
{
    for (int i = 0; i < (sizeof(__HTTP_CODE_MESSAGES) / sizeof(__HTTP_CODE_MESSAGES[0])); i++)
    {
        if (code == __HTTP_CODE_MESSAGES[i].code)
        {
            return __HTTP_CODE_MESSAGES[i].message;
        }
    }
    return "OK";
}