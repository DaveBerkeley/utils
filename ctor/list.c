
#include <stdlib.h>

#include "list.h"

void list_init(List *head)
{
    head->lock = lock_init();
}

ListItem* list_insert(List *head, void *data)
{
    ListItem *item = (ListItem*) malloc(sizeof(*item));
    if (!item)
    {
        return 0;
    }

    lock_lock(head->lock);

    item->data = data;
    item->next = head->list;
    head->list = item;

    lock_unlock(head->lock);
    return item;
}

void* list_pop(List *head)
{
    ListItem *item = 0;
    void *data = 0;

    lock_lock(head->lock);

    item = head->list;
    if (item)
    {
        head->list = item->next;
        item->next = 0;

        data = item->data;
        free(item);
    }

    lock_unlock(head->lock);
    return data;
}


