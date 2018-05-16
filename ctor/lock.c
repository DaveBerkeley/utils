
#include <assert.h>

#include "lock.h"

struct Lock
{
};

struct Lock* lock_init()
{
    //  TODO
    static struct Lock lock;

    return & lock;
}

void lock_lock(struct Lock *lock)
{
    assert(lock);
}

void lock_unlock(struct Lock *lock)
{
    assert(lock);
}

//  FIN
