
#include "log.h"

#include "callback.h"

static void _callback_lock(Callbacks *cbs)
{
    //  TODO
}

static void _callback_unlock(Callbacks *cbs)
{
    //  TODO
}

void callback_run(Callbacks *cbs, void *args)
{
    _callback_lock(cbs);

    for (Callback *cb = cbs->callbacks; cb; cb = cb->next)
    {
        XLOG_DEBUG("run %s", cb->name);
        cb->fn(cb->arg, args);
    }

    _callback_unlock(cbs);
}

void callback_add(Callbacks *cbs, Callback *cb)
{
    _callback_lock(cbs);

    XLOG_DEBUG("add %p %p %s", cb->fn, cb->arg, cb->name);
    cb->next = cbs->callbacks;
    cbs->callbacks = cb;

    _callback_unlock(cbs);
}

void callback_remove(Callbacks *cbs, Callback *cb)
{
    _callback_lock(cbs);

    for (Callback **head = & cbs->callbacks; *head; head = &((*head)->next))
    {
        if (cb == *head)
        {
            *head = cb->next;
            cb->next = 0;
            break;
        }
    }

    _callback_unlock(cbs);
}

void callback_remove_all(Callbacks *cbs)
{
    while (cbs->callbacks)
    {
        callback_remove(cbs, cbs->callbacks);
    }
}

//  FIN
