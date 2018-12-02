
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>

#define ASSERT(x) assert(x)

typedef pthread_mutex_t *mutex;

struct Waiter;

typedef struct Waiter
{
    //  Wait for timeout or post

    //  Used by Semaphore to list waiters
    struct Waiter *next_wait;

    //  Used by Timer to list timed waiters
    struct Waiter *next_timed;
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
    Waiter **next = pnext(w, wait);
    *next = *head;
    *head = w;
}

void waiter_append(Waiter **head, Waiter *w, enum WaitType wait)
{
    while (*head)
    {
        head = pnext(w, wait);
    }
    *head = w;
    *pnext(w, wait) = 0;
}

    /*
     *
     */

typedef struct
{
    //  Handle post()
    //  has list of waiter objects waiting on this Semaphore
}   Semaphore;

typedef struct
{
    //  Single timer object
    //  Has list of all Waiter objects doing timed wait
}   Timer;

    /*
     *
     */

int main()
{
    return 0;
}

// FIN
