
#if ! defined(__LOG_H__)

#define __LOG_H__

#include <syslog.h>

typedef struct LogInfo {
    struct LogInfo *next;
    int level;
    const char *path;
}   LogInfo;

void __log(LogInfo *log, int level, const char* fmt, ...) __attribute__((format(printf,3,4)));

#define LOG(level, fmt, ...) \
        __log(& log_state, level, " %s():%d : " fmt, \
                __FUNCTION__, __LINE__, ## __VA_ARGS__ )

#define XLOG_INFO(fmt, ...) LOG(LOG_WARN, fmt, ## __VA_ARGS__ )
#define XLOG_DEBUG(fmt, ...) LOG(LOG_DEBUG, fmt, ## __VA_ARGS__ )
#define XLOG_ERROR(fmt, ...) LOG(LOG_ERR, fmt, ## __VA_ARGS__ )

void log_register(LogInfo *state, const char *path);
void log_visit(void (*fn)(const LogInfo *state, void *arg), void *arg);

#define __STRINGISE(x) #x
#define STRINGISE(x) __STRINGISE(x)

#define LOGGING \
static LogInfo log_state; \
 \
__attribute__((constructor)) \
static void log_test() \
{ \
    log_register(& log_state, STRINGISE(__CWD__) "/" __FILE__); \
}

#endif // __LOG_H__
