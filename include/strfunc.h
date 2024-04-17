#ifndef _STRFUNC_H
#define _STRFUNC_H

#include <string.h>
#include <stdlib.h>

// counts instances of needle in haystack
int strcount(char *haystack, char *needle);

// replaces all instances of needle in haystack with new_needle
char *strreplace(char *haystack, char *needle, char *new_needle, int free_old_haystack);

// gets rest of haystack after last occurance of needle
char *strafterlast(const char *haystack, const char *needle);

#endif /* _STRFUNC_H */