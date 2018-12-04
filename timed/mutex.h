
#if !defined(__MUTEX_H__)

#define __MUTEX_H__

#include <pthread.h>
#include <stdbool.h>

#include "environment.h"

typedef struct 
{
    pthread_mutex_t mutex;
    int locked;
}   Mutex;

#define MUTEX_INIT { PTHREAD_MUTEX_INITIALIZER, false }

static inline void lock(Mutex *mutex)
{
    if (mutex)
    {
        const int err = pthread_mutex_lock(& mutex->mutex);
        ASSERT(err == 0);
        mutex->locked += 1;
    }
}

static inline void unlock(Mutex *mutex)
{
    if (mutex)
    {
        mutex->locked -= 1;
        const int err = pthread_mutex_unlock(& mutex->mutex);
        ASSERT(err == 0);
    }
}

#endif // __MUTEX_H__

//  FIN
