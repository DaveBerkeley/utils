
#include "mutex.h"

#include "list.h"

static void _list_insert(void **head, void *w, void **next)
{
    ASSERT(head);
    ASSERT(w);
    ASSERT(next);

    *next = *head;
    *head = w;
}

int list_size(void **head, pnext next_fn, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(next_fn);

    int count = 0;

    lock(mutex);

    for (; *head; head = next_fn(*head))
    {
        count += 1;
    }

    unlock(mutex);

    return count;
}

void list_push(void **head, void *w, pnext next_fn, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(w);
    ASSERT(next_fn);

    lock(mutex);

    _list_insert(head, w, next_fn(w));

    unlock(mutex);
}

void list_append(void **head, void *w, pnext next_fn, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(w);
    ASSERT(next_fn);

    lock(mutex);

    while (*head)
    {
        head = next_fn(*head);
    }

    _list_insert(head, w, next_fn(w));

    unlock(mutex);
}

void list_add_sorted(void **head, void *w, pnext next_fn, cmp_fn cmp, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(w);
    ASSERT(next_fn);
    ASSERT(cmp);

    lock(mutex);

    for (; *head; head = next_fn(*head))
    {
        void *item = *head;

        if (cmp(w, item) >= 0)
        {
            break;
        }
    }

    _list_insert(head, w, next_fn(w));

    unlock(mutex);
}

void *list_pop(void **head, pnext next_fn, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(next_fn);

    lock(mutex);

    void *w = *head;

    if (w)
    {
        void** next = next_fn(w);
        *head = *next;
        *next = 0;
    }

    unlock(mutex);

    return w;
}

bool list_remove(void **head, void *w, pnext next_fn, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(w);
    ASSERT(next_fn);

    bool found = false;
    lock(mutex);

    for (; *head; head = next_fn(*head))
    {
        if (w == *head)
        {
            // unlink top item
            list_pop(head, next_fn, 0);
            found = true;
            break;
        }
    }

    unlock(mutex);
    return found;
}

void list_visit(void **head, pnext next_fn, visitor fn, void *arg, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(next_fn);
    ASSERT(fn);

    lock(mutex);

    for (; *head; head = next_fn(*head))
    {
        void *item = *head;

        if (fn(item, arg))
        {
            break;
        }
    }

    unlock(mutex);
}

//  FIN

