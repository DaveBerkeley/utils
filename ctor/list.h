
#if ! defined(__LIST_H__)

#define __LIST_H__

#include "lock.h"

typedef struct ListItem {
    struct ListItem *next;
    void *data;
}   ListItem;

typedef struct 
{
    ListItem *list;
    struct Lock *lock;
}   List;

void list_init(List *head);

ListItem* list_insert(List *head, void *data);
void* list_pop(List *head);

#endif // __LIST_H__

