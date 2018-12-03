
#include "mutex.h"

#include "list.h"

typedef void** (*pnext)(void *item);

static void _list_insert(void **head, void *w, void **next)
{
    *next = *head;
    *head = w;
}

void list_insert(void **head, void *w, pnext next_fn, Mutex *mutex)
{
    lock(mutex);

    _list_insert(head, w, next_fn(w));

    unlock(mutex);
}

void list_append(void **head, void *w, pnext next_fn, Mutex *mutex)
{
    lock(mutex);

    while (*head)
    {
        head = next_fn(*head);
    }

    _list_insert(head, w, next_fn(w));

    unlock(mutex);
}

typedef int (*cmp_fn)(const void *, const void *);

void list_add(void **head, void *w, pnext next_fn, cmp_fn cmp, Mutex *mutex)
{
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

void list_remove(void **head, void *w, pnext next_fn, Mutex *mutex)
{
    lock(mutex);

    for (; *head; head = next_fn(*head))
    {
        void *item = *head;

        if (item == w)
        {
            // unlink this item
            void **next = next_fn(w);
            *head = *next;
            *next = 0;
            break;
        }
    }

    unlock(mutex);
}

//  FIN

