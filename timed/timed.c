
#include <stdbool.h>
#include <semaphore.h>
#include <assert.h>
#include <string.h>

#include "timed.h"

    /*
     *
     */

struct Semaphore;

typedef struct Waiter
{
    //  Wait for timeout or post
    sem_t sem;

    //  Used by Semaphore to list waiters
    struct Waiter *next_wait;

    //  Used by Timer to list timed waiters
    struct Waiter *next_timed;

    //  Expiry time
    struct timespec expire;

    //  Return code
    int code;
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

static void _waiter_insert(Waiter **head, Waiter *w, Waiter **next)
{
    *next = *head;
    *head = w;
}

void waiter_insert(Waiter **head, Waiter *w, enum WaitType wait, Mutex *mutex)
{
    lock(mutex);

    _waiter_insert(head, w, pnext(w, wait));

    unlock(mutex);
}

void waiter_append(Waiter **head, Waiter *w, enum WaitType wait, Mutex *mutex)
{
    lock(mutex);

    while (*head)
    {
        head = pnext(*head, wait);
    }

    _waiter_insert(head, w, pnext(w, wait));

    unlock(mutex);
}

typedef int (*cmp_fn)(const Waiter *, const Waiter *);

void waiter_add(Waiter **head, Waiter *w, enum WaitType wait, cmp_fn fn, Mutex *mutex)
{
    lock(mutex);

    for (; *head; head = pnext(*head, wait))
    {
        Waiter *item = *head;

        if (fn(w, item) >= 0)
        {
            break;
        }
    }

    _waiter_insert(head, w, pnext(w, wait));

    unlock(mutex);
}

void waiter_remove(Waiter **head, Waiter *w, enum WaitType wait, Mutex *mutex)
{
    lock(mutex);

    for (; *head; head = pnext(*head, wait))
    {
        Waiter *item = *head;

        if (item == w)
        {
            // unlink this item
            Waiter **next = pnext(w, wait);
            *head = *next;
            *next = 0;
            break;
        }
    }

    unlock(mutex);
}

static int cmp_time(const Waiter *w1, const Waiter *w2)
{
    const struct timespec *t1 = & w1->expire;
    const struct timespec *t2 = & w2->expire;

    if (t1->tv_sec == t2->tv_sec)
    {
        return t2->tv_nsec - t1->tv_nsec;
    }

    return t2->tv_sec - t1->tv_sec;
}

    /*
     *
     */

typedef struct
{
    //  Single timer object
    //  Has list of all Waiter objects doing timed wait
    Waiter *waiters;
    Mutex mutex;
}   Timer;

static Timer timer;

    /*
     *
     */

int semaphore_timed_wait(Semaphore *s, struct timespec *t)
{
    Waiter w;

    memset(& w, 0, sizeof(w));
    sem_init(& w.sem, 0, 0);

    if (t)
    {
        w.expire = *t;
        //  Append to Timer list
        waiter_add(& timer.waiters, & w, WAIT_TIMED, cmp_time, & timer.mutex);
        //  TODO : Reshedule timer?
    }

    // Append to S's list
    waiter_append(& s->waiters, & w, WAIT_BLOCK, & s->mutex);

    sem_wait(& w.sem);

    // Remove from the list
    waiter_remove(& s->waiters, & w, WAIT_BLOCK, & s->mutex);

    if (t)
    {
        //  Remove from Timer list
        waiter_remove(& timer.waiters, & w, WAIT_TIMED, & timer.mutex);
    }

    sem_destroy(& w.sem);

    return w.code;
}

int semaphore_wait(Semaphore *s)
{
    return semaphore_timed_wait(s, 0);
}

    /*
     *
     */

int main()
{
    return 0;
}

// FIN
