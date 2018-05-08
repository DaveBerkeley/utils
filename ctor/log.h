
#if ! defined(__LOG_H__)

#define __LOG_H__

#include <syslog.h>

void __log(int level, const char* fmt, ...) __attribute__((format(printf,2, 3)));

#define LOG(level, fmt, ...) \
        __log(level, "%s %s():%d : " fmt, \
                __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__ )

#define XLOG_INFO(fmt, ...) LOG(LOG_WARN, fmt, ## __VA_ARGS__ )
#define XLOG_DEBUG(fmt, ...) LOG(LOG_DEBUG, fmt, ## __VA_ARGS__ )
#define XLOG_ERROR(fmt, ...) LOG(LOG_ERR, fmt, ## __VA_ARGS__ )

#endif // __LOG_H__
