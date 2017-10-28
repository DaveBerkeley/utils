
#include "mutex.h"

#include "lock.h"

Lock::Lock(Mutex& m)
: mutex(m)
{
    mutex.lock();
}

Lock::~Lock()
{
    mutex.unlock();
}

//  FIN
