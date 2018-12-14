
#include <stdbool.h>
#include <semaphore.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "mutex.h"
#include "list.h"
#include "timed.h"

    /*
     *
     */

typedef struct Waiter
{
    //  Wait for timeout or post
    sem_t sem;

    //  The owning Semaphore
    Semaphore *semaphore;

    //  Used by Semaphore to list waiters
    struct Waiter *next_wait;

    //  Used by Timer to list timed waiters
    struct Waiter *next_timed;

    //  Expiry time
    struct timespec expire;
    bool timed;

    //  Return code
    enum semaphore_code code;
}   Waiter;

    /*
     *
     */

static int cmp_time(const struct timespec *t1, const struct timespec *t2)
{
    if (t1->tv_sec == t2->tv_sec)
    {
        return t2->tv_nsec - t1->tv_nsec;
    }

    return t2->tv_sec - t1->tv_sec;
}

static int cmp_waiter(const pList a1, const pList a2)
{
    const Waiter *w1 = (Waiter*) a1;
    const Waiter *w2 = (Waiter*) a2;
    const struct timespec *t1 = & w1->expire;
    const struct timespec *t2 = & w2->expire;

    return cmp_time(t1, t2);
}

    /*
     *  Helper functions used by list_*() library
     */

static pList* pnext_wait(pList v)
{
    Waiter *w = (Waiter*) v;
    return (pList*) & w->next_wait;
}

static pList* pnext_timed(pList v)
{
    Waiter *w = (Waiter*) v;
    return (pList*) & w->next_timed;
}

static void waiter_post(Waiter *waiter, enum semaphore_code code)
{
    //  post the waiter
    waiter->code = code;
    sem_post(& waiter->sem);
}

    /*
     *
     */

static void semaphore_append(Semaphore *s, Waiter *w, Mutex *mutex)
{
    list_append((pList*) & s->waiters, (pList) w, pnext_wait, mutex);
}

static bool semaphore_remove(Waiter *w)
{
    Semaphore *s = w->semaphore;
    return list_remove((pList*) & s->waiters, (pList) w, pnext_wait, & s->mutex);
}

static Waiter * semaphore_pop(Semaphore *s, Mutex *mutex)
{
    return (Waiter*) list_pop((pList*) & s->waiters, pnext_wait, mutex);
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

static void timer_lock()
{
    Timer *t = & timer;
    lock(& t->mutex);
}

static void timer_unlock()
{
    Timer *t = & timer;
    unlock(& t->mutex);
}

static Waiter * timer_peek()
{
    Timer *t = & timer;
    ASSERT(t->mutex.locked);
    return t->waiters;
}

static Waiter * timer_pop()
{
    Timer *t = & timer;
    ASSERT(t->mutex.locked);
    return (Waiter*) list_pop((pList*) & t->waiters, pnext_timed, 0);
}

static void timer_reschedule()
{
    Timer *t = & timer;
    ASSERT(t->mutex.locked);

    // wait for next event, if there is one
    Waiter *waiter = timer_peek();
    // TODO
    // check if a reshedule is needed
}

static void timer_add(Waiter *w)
{
    Timer *t = & timer;
    ASSERT(t->mutex.locked);

    list_add_sorted((pList*) & t->waiters, (pList) w, pnext_timed, cmp_waiter, 0);
    timer_reschedule();
}

static void timer_remove(Waiter *w)
{
    if (!w->timed)
    {
        // no pending wait
        return;
    }

    Timer *t = & timer;
    lock(& t->mutex);

    list_remove((pList*) & t->waiters, (pList) w, pnext_timed, 0);
    timer_reschedule();

    unlock(& t->mutex);
}

static void timer_expire()
{
    //  wake any waiters
    Timer *t = & timer;
    Mutex *mutex = & t->mutex;
    bool changed = false;

    struct timespec now;
    clock_gettime(CLOCK_REALTIME_COARSE, & now);

    lock(mutex);

    //  post all the expired waiters
    while (true)
    {
        Waiter *waiter = timer_peek();

        if (!waiter)
        {
            // no more waiters
            break;
        }

        if (cmp_time(& now, & waiter->expire) < 0)
        {
            // no expired waiters
            break;
        }

        waiter = timer_pop();
        ASSERT(waiter);

        // if remove fails, then is must have already been posted
        if (semaphore_remove(waiter))
        {
            waiter_post(waiter, SEMAPHORE_TIMEOUT);
        }

        changed = true;
    }

    if (changed)
    {
        timer_reschedule();
    }

    unlock(mutex);
}

    /*
     *
     */

enum semaphore_code semaphore_timed_wait(Semaphore *s, struct timespec *t)
{
    Waiter w;
    memset(& w, 0, sizeof(w));

    w.semaphore = s;
    sem_init(& w.sem, 0, 0);

    w.code = SEMAPHORE_ERROR;

    if (t)
    {
        w.expire = *t;
        w.timed = true;
        timer_lock();

        //  these must be atomic, in case timer goes off immeadiately
        timer_add(& w);
        semaphore_append(s, & w, & s->mutex);

        timer_unlock();
    }
    else
    {
        semaphore_append(s, & w, & s->mutex);
    }

    // TODO : check the semaphore count?
    // might be possible to exit immeadiately

    int err = sem_wait(& w.sem);
    if (err < 0)
    {
        // check return code
        if (errno == EINTR)
        {
            w.code = SEMAPHORE_SIGNAL;
        }
        else
        {
            w.code = SEMAPHORE_ERROR;
        }
    }

    sem_destroy(& w.sem);

    return w.code;
}

enum semaphore_code semaphore_wait(Semaphore *s)
{
    return semaphore_timed_wait(s, 0);
}

    /*
     *
     */

int semaphore_post(Semaphore *s)
{
    // pop/post the first waiter, or change the count
    Mutex *mutex = & s->mutex;

    lock(mutex);

    Waiter *waiter = semaphore_pop(s, 0);

    if (waiter)
    {
        //  we have a waiter
        timer_remove(waiter);
        waiter_post(waiter, SEMAPHORE_POST);
    }
    else
    {
        //  no waiter, so increment the counter
        s->count += 1;
    }

    unlock(mutex);
    return 0;
}

// FIN
