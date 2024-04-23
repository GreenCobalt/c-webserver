#include "include/file.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int file_exists(char *path)
{
    return access(path, F_OK) == 0;
}

file_content read_file(char *path, magic_t magic)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        file_content r = {
            .exists = 0,
        };
        return r;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    file_content r;
    char *string = calloc(fsize, sizeof(char));
    if (fread(string, fsize, 1, f))
    {
        r.content = string;
        r.mime_type = magic_file(magic, path);
        r.exists = 1;
        r.size = fsize;
    }

    fclose(f);
    return r;
}