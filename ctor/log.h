
#if ! defined(__LOG_H__)

#define __LOG_H__

#include <syslog.h>
#include <stdbool.h>

#define __STRINGISE(x) #x
#define STRINGISE(x) __STRINGISE(x)

typedef struct LogInfo {
    struct LogInfo *next;
    int level;
    const char *path;
}   LogInfo;

void __log(LogInfo *log, int level, const char* fmt, ...) __attribute__((format(printf,3,4)));

#define __log_test(a,b) (true) // TODO

#define LOG(level, fmt, ...) \
    if (__log_test(& log_state, level)) { \
        log_state.path = STRINGISE(__CWD__) "/" __FILE__; \
        __log(& log_state, level, " +%d %s() : " fmt, \
                __LINE__, __FUNCTION__, ## __VA_ARGS__ ); \
    }

#define XLOG_INFO(fmt, ...) LOG(LOG_WARN, fmt, ## __VA_ARGS__ )
#define XLOG_DEBUG(fmt, ...) LOG(LOG_DEBUG, fmt, ## __VA_ARGS__ )
#define XLOG_ERROR(fmt, ...) LOG(LOG_ERR, fmt, ## __VA_ARGS__ )

void __log_register(LogInfo *state);

void log_visit(void (*fn)(const LogInfo *state, void *arg), void *arg);
void log_set_level(LogInfo *state, int level);
int log_get_level(LogInfo *state);

    /*
     *  You shouldn't generally put a static variable into a header file,
     *  but we want every module that includes log.h to define
     *  a LogInfo and an init function that gets called prior to main. 
     */

static LogInfo log_state;

__attribute__((constructor))
static void __log_init()
{
    __log_register(& log_state);
}

#endif // __LOG_H__
