
#ifndef X_MUTEX_H
#define X_MUTEX_H

#include <pthread.h>

class Mutex
{
private:
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;

public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();
};

#endif  //  X_MUTEX_H

