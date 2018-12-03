
#include <pthread.h>

#include "environment.h"

typedef pthread_mutex_t Mutex;

static inline void lock(Mutex *mutex)
{
    if (mutex)
    {
        const int err = pthread_mutex_lock(mutex);
        ASSERT(err == 0);
    }
}

static inline void unlock(Mutex *mutex)
{
    if (mutex)
    {
        const int err = pthread_mutex_unlock(mutex);
        ASSERT(err == 0);
    }
}

//  FIN
