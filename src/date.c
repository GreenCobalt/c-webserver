#include "include/date.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

const char *DAY[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *MONTH[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

char *generate_date_string()
{
    char *res = calloc(40, sizeof(char));
    time_t t = time(NULL);
    struct tm tm = *gmtime(&t);
    snprintf(res, 40, "%s, %02d %s %04d %02d:%02d:%02d %s", DAY[tm.tm_wday], tm.tm_mday, MONTH[tm.tm_mon], tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, "GMT");
    return res;
}