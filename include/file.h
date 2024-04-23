#ifndef FILE_H_
#define FILE_H_

#include "include/def.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <magic.h>

int file_exists(char *path);
file_content read_file(char *path, magic_t magic);

#endif /* FILE_H_ */