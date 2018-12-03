
#include <stdbool.h>
#include <semaphore.h>
#include <assert.h>
#include <string.h>

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
    int code;
}   Waiter;

    /*
     *
     */

static int cmp_time(const void *a1, const void *a2)
{
    const Waiter *w1 = (Waiter*) a1;
    const Waiter *w2 = (Waiter*) a2;
    const struct timespec *t1 = & w1->expire;
    const struct timespec *t2 = & w2->expire;

    if (t1->tv_sec == t2->tv_sec)
    {
        return t2->tv_nsec - t1->tv_nsec;
    }

    return t2->tv_sec - t1->tv_sec;
}

    /*
     *  Helper functions used by list_*() library
     */

static void ** pnext_wait(void *v)
{
    Waiter *w = (Waiter*) v;
    return (void**) & w->next_wait;
}

static void ** pnext_timed(void *v)
{
    Waiter *w = (Waiter*) v;
    return (void**) & w->next_timed;
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

static void timer_reschedule()
{
    Timer *t = & timer;
    //  TODO
    ASSERT(t->mutex.locked);
}

static void timer_add(Waiter *w)
{
    Timer *t = & timer;
    lock(& t->mutex);

    list_add_sorted((void**) & t->waiters, w, pnext_timed, cmp_time, 0);
    timer_reschedule();

    unlock(& t->mutex);
}

static void timer_remove(Waiter *w)
{
    Timer *t = & timer;
    lock(& t->mutex);

    list_remove((void**) & t->waiters, w, pnext_timed, 0);
    timer_reschedule();

    unlock(& t->mutex);
}

    /*
     *
     */

static void semaphore_append(Semaphore *s, Waiter *w)
{
    list_append((void**) & s->waiters, & w, pnext_wait, & s->mutex);
}

static void semaphore_remove(Semaphore *s, Waiter *w)
{
    list_remove((void**) & s->waiters, w, pnext_wait, & s->mutex);
}

    /*
     * Called when either Timer or Semaphore need to post
     */

static void waiter_post(Waiter *w, int code, bool is_timer)
{
    // unlink from Semaphore
    if (is_timer)
    {
        semaphore_remove(w->semaphore, w);
    }
    // unlink from Timer
    if (w->timed && !is_timer)
    {
        timer_remove(w);
    }
    // set the return code
    w->code = code;
    // post semaphore
    sem_post(& w->sem);
}

    /*
     *
     */

int semaphore_timed_wait(Semaphore *s, struct timespec *t)
{
    Waiter w;
    memset(& w, 0, sizeof(w));

    w.semaphore = s;
    sem_init(& w.sem, 0, 0);

    if (t)
    {
        w.expire = *t;
        w.timed = true;
        timer_add(& w);
    }

    semaphore_append(s, & w);

    // TODO : check posix return code
    sem_wait(& w.sem);

    sem_destroy(& w.sem);

    return w.code;
}

int semaphore_wait(Semaphore *s)
{
    return semaphore_timed_wait(s, 0);
}

// FIN
