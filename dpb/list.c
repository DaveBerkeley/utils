
#include "mutex.h"

#include "list.h"

static void _list_insert(pList* head, pList w, pList *next)
{
    ASSERT(head);
    ASSERT(w);
    ASSERT(next);

    *next = *head;
    *head = w;
}

int list_size(pList *head, pnext next_fn, Mutex *mutex)
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

void list_push(pList *head, pList w, pnext next_fn, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(w);
    ASSERT(next_fn);

    lock(mutex);

    _list_insert(head, w, next_fn(w));

    unlock(mutex);
}

void list_append(pList *head, pList w, pnext next_fn, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(w);
    ASSERT(next_fn);

    lock(mutex);

    for (; *head; head = next_fn(*head))
    {
        ;
    }

    _list_insert(head, w, next_fn(w));

    unlock(mutex);
}

void list_add_sorted(pList *head, pList w, pnext next_fn, cmp_fn cmp, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(w);
    ASSERT(next_fn);
    ASSERT(cmp);

    lock(mutex);

    for (; *head; head = next_fn(*head))
    {
        if (cmp(w, *head) >= 0)
        {
            break;
        }
    }

    _list_insert(head, w, next_fn(w));

    unlock(mutex);
}

pList list_pop(pList *head, pnext next_fn, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(next_fn);

    lock(mutex);

    pList w = *head;

    if (w)
    {
        pList* next = next_fn(w);
        *head = *next;
        *next = 0;
    }

    unlock(mutex);

    return w;
}

bool list_remove(pList *head, pList w, pnext next_fn, Mutex *mutex)
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

pList list_find(pList *head, pnext next_fn, visitor fn, void *arg, Mutex *mutex)
{
    ASSERT(head);
    ASSERT(next_fn);
    ASSERT(fn);

    pList item = 0;

    lock(mutex);

    for (; *head; head = next_fn(*head))
    {
        if (fn(*head, arg))
        {
            item = *head;
            break;
        }
    }

    unlock(mutex);

    return item;
}

void list_visit(pList *head, pnext next_fn, visitor fn, void *arg, Mutex *mutex)
{
    list_find(head, next_fn, fn, arg, mutex);
}

//  FIN

