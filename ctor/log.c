
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "log.h"

void __log(LogInfo *info, int level, const char* fmt, ...)
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
    printf("%s %s", buff, info->path);

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    printf("\n");
}

static LogInfo *files = 0;

void __log_register(LogInfo *state)
{
    XLOG_DEBUG("");
    state->next = files;
    files = state;
}

void log_visit(void (*fn)(const LogInfo *state, void *arg), void *arg)
{
    for (const LogInfo *state = files; state; state = state->next)
    {
        fn(state, arg);
    }
}

void log_set_level(LogInfo *state, int level)
{
    state->level = level;
}

int log_get_level(LogInfo *state)
{
    return state->level;
}

//  FIN
