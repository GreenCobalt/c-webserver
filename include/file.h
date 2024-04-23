#ifndef FILE_H_
#define FILE_H_

#include "include/http_def.h"
#include <magic.h>

int file_exists(char *path);
file_content read_file(char *path, magic_t magic);

#endif /* FILE_H_ */