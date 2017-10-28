
#ifndef X_MUTEX_H
#define X_MUTEX_H

#include <pthread.h>

class Mutex
{
private:
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;

public:
    Mutex()
    {
        pthread_mutexattr_init(& attr);
        pthread_mutexattr_settype(& attr, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_init(& mutex, & attr); 
    }

    ~Mutex()
    {
        pthread_mutex_destroy(& mutex);
    }

    void lock()
    {
        pthread_mutex_lock(& mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(& mutex);
    }
};

#endif  //  X_MUTEX_H

