#include "include/file.h"

file_content read_file(char *path)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        char *string = calloc(1, sizeof(char));
        string = 0;

        file_content r = {
            content : string,
            size : 0
        };
        return r;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = calloc(fsize + 1, sizeof(char));
    fread(string, fsize, 1, f);
    fclose(f);
    string[fsize] = 0;

    file_content r;
    r.content = string;
    r.size = fsize + 1;

    return r;
}