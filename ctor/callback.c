
#include "log.h"

#include "callback.h"

void callback_init(Callbacks *cbs)
{
    cbs->lock = lock_init();
}

void callback_run(Callbacks *cbs, void *args)
{
    if (!cbs->lock)
    {
        callback_init(cbs);
    }

    lock_lock(cbs->lock);

    XLOG_DEBUG("run");

    for (Callback *cb = cbs->callbacks; cb; cb = cb->next)
    {
        XLOG_DEBUG("run %s", cb->name);
        cb->fn(cb->arg, args);
    }

    lock_unlock(cbs->lock);
}

void callback_add(Callbacks *cbs, Callback *cb)
{
    if (!cbs->lock)
    {
        callback_init(cbs);
    }

    lock_lock(cbs->lock);

    XLOG_DEBUG("add %p %p %s", cb->fn, cb->arg, cb->name);
    cb->next = cbs->callbacks;
    cbs->callbacks = cb;

    lock_unlock(cbs->lock);
}

void callback_remove(Callbacks *cbs, Callback *cb)
{
    lock_lock(cbs->lock);

    for (Callback **head = & cbs->callbacks; *head; head = &((*head)->next))
    {
        if (cb == *head)
        {
            *head = cb->next;
            cb->next = 0;
            break;
        }
    }

    lock_unlock(cbs->lock);
}

void callback_remove_all(Callbacks *cbs)
{
    while (cbs->callbacks)
    {
        callback_remove(cbs, cbs->callbacks);
    }
}

//  FIN
