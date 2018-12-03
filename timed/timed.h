
#include "mutex.h"

    /*
     *
     */

struct Waiter;

typedef struct Semaphore
{
    //  Handle post()

    //  TODO : List of active semaphores
    struct Semaphore *next;

    //  list of waiter objects blocking on this Semaphore
    struct Waiter *waiters;
    //  lock for the list
    Mutex mutex;
}   Semaphore;


