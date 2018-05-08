
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "log.h"

void __log(int level, const char* fmt, ...)
{
    char buff[32];
    va_list ap;

    time_t tt;
    time(& tt);
    ctime_r(& tt, buff);
    char* end = index(buff, '\n');
    if (end)
    {
        *end = '\0';
    }
    printf("%s ", buff);

    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

