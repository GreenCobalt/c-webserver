#include "include/file.h"

file_content read_file(char *path)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        char *string = malloc(sizeof(char));
        *string = '\0';

        file_content r = {
            .content = string,
            .size = 0
        };
        return r;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    file_content r;
    char *string = calloc(fsize + 1, sizeof(char));
    if (fread(string, fsize, 1, f))
    {
        string[fsize] = 0;

        r.content = string;
        r.size = fsize + 1;
    }
    fclose(f);
    return r;
}