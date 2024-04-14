#include "include/strfunc.h"

int strcount(char *haystack, char *needle)
{
    char *current_loc = haystack;
    int needle_len = strlen(needle);
    int count = 0;

    if (needle_len)
    {
        while (current_loc = strstr(current_loc, needle))
        {
            current_loc += needle_len;
            count++;
        }
        return count;
    }
    else
    {
        return 0;
    }
}

char *strreplace(char *haystack, char *needle, char *new_needle)
{
    if (needle == "")
        return haystack;

    int needle_len_diff = strlen(new_needle) - strlen(needle);
    long new_haystack_size = strlen(haystack) - (strcount(haystack, needle) * (-1 * needle_len_diff));

    char *new_haystack = malloc(new_haystack_size * sizeof(char));
    char *last_strstr = strstr(haystack, needle);
    char *last_replacement_end = haystack;

    int repcount = 0;
    int addedchars = 0;
    while (last_strstr)
    {
        // add replacement characters to new_haystack
        strncpy(&new_haystack[(last_strstr - haystack) + addedchars], new_needle, strlen(new_needle));

        // add non-replaced characters between previous replacement location and this one to new_haystack
        strncpy(&new_haystack[last_replacement_end - haystack + addedchars], last_replacement_end, last_strstr - last_replacement_end);

        last_replacement_end = last_strstr + strlen(needle);
        last_strstr = strstr(last_replacement_end, needle);

        repcount++;
        addedchars = repcount * needle_len_diff;
    }
    // add non-replaced characters between last replacement and end of string to new_haystack
    strncpy(&new_haystack[last_replacement_end - haystack + (repcount * needle_len_diff)], last_replacement_end, strlen(haystack) - (last_replacement_end - haystack));

    if (new_haystack[new_haystack_size] != '\0')
        new_haystack[new_haystack_size] = '\0';
    return new_haystack;
}

/*
#include <stdio.h>
int main() {
    printf("%s\n", strreplace("fdsfdsf", "f", "AA"));
}
*/