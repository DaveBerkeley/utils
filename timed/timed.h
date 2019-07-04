
#if !defined(__TIMED_H__)

#define __TIMED_H__

#include <mutex.h>

    /*
     *
     */

struct Waiter;

enum semaphore_code
{
    SEMAPHORE_POST = 1,
    SEMAPHORE_TIMEOUT,
    SEMAPHORE_SIGNAL,
    SEMAPHORE_ERROR,
};  

typedef struct Semaphore
{
    int count;

    //  list of active semaphores
    struct Semaphore *next;

    //  list of waiter objects blocking on this Semaphore
    struct Waiter *waiters;
    //  lock for the list
    Mutex mutex;
}   Semaphore;

#define SEMAPHORE_INIT { 0, 0, 0, MUTEX_INIT }

int semaphore_post(Semaphore *s);
enum semaphore_code semaphore_wait(Semaphore *s);
enum semaphore_code semaphore_timed_wait(Semaphore *s, struct timespec *t);

#endif // __TIMED_H__

//  FIN
