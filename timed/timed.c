
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <string.h>

#define ASSERT(x) assert(x)

typedef pthread_mutex_t Mutex;

static inline void lock(Mutex *mutex)
{
    if (mutex)
    {
        pthread_mutex_lock(mutex);
        // TODO : check error
    }
}

static inline void unlock(Mutex *mutex)
{
    if (mutex)
    {
        pthread_mutex_unlock(mutex);
        // TODO : check error
    }
}

struct Semaphore;

typedef struct Waiter
{
    //  Wait for timeout or post
    Mutex *mutex;

    sem_t sem;

    //  Used by Semaphore to list waiters
    struct Waiter *next_wait;

    //  Used by Timer to list timed waiters
    struct Waiter *next_timed;

    //  Expiry time
    struct timespec expire;
}   Waiter;

    /*
     *
     */

enum WaitType {
    WAIT_BLOCK = 1,
    WAIT_TIMED,
};

static inline struct Waiter **pnext(Waiter *w, enum WaitType wait)
{
    switch (wait)
    {
        case WAIT_BLOCK : return & w->next_wait;
        case WAIT_TIMED : return & w->next_timed;
        default : ASSERT(0);
    }
}

void waiter_insert(Waiter **head, Waiter *w, enum WaitType wait)
{
    lock(w->mutex);

    Waiter **next = pnext(w, wait);
    *next = *head;
    *head = w;

    unlock(w->mutex);
}

void waiter_append(Waiter **head, Waiter *w, enum WaitType wait)
{
    lock(w->mutex);

    while (*head)
    {
        head = pnext(w, wait);
    }
    *head = w;
    *pnext(w, wait) = 0;

    unlock(w->mutex);
}

void waiter_remove(Waiter **head, Waiter *w, enum WaitType wait)
{
    lock(w->mutex);

    for (; *head; head = pnext(w, wait))
    {
        Waiter *item = *head;

        if (item == w)
        {
            Waiter **next = pnext(w, wait);
            *head = *next;
            *next = 0;
            break;
        }
    }

    unlock(w->mutex);
}

//  TODO : insert sorted

    /*
     *
     */

typedef struct
{
    //  Handle post()


    //  list of waiter objects waiting on this Semaphore
    Waiter *waiters;
    //  lock for the list
    Mutex mutex;
}   Semaphore;

int semaphore_wait(Semaphore *s)
{
    Waiter w;

    memset(& w, 0, sizeof(w));
    w.mutex = & s->mutex;

    sem_init(& w.sem, 0, 0);

    // Append to S's list
    waiter_append(& s->waiters, & w, WAIT_BLOCK);

    sem_wait(& w.sem);

    // Remove from the list
    waiter_remove(& s->waiters, & w, WAIT_BLOCK);

    sem_destroy(& w.sem);

    return 1234; // TODO
}

    /*
     *
     */

typedef struct
{
    //  Single timer object
    //  Has list of all Waiter objects doing timed wait
    Mutex mutex;
}   Timer;

    /*
     *
     */

int main()
{
    return 0;
}

// FIN
