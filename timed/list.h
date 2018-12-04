
#if !defined(__LIST_H__)

#define __LIST_H__

#include "mutex.h"

typedef void** (*pnext)(void *item);

void list_push(void **head, void *w, pnext next_fn, Mutex *mutex);
void list_append(void **head, void *w, pnext next_fn, Mutex *mutex);
bool list_remove(void **head, void *w, pnext next_fn, Mutex *mutex);
int list_size(void **head, pnext next_fn, Mutex *mutex);

void *list_pop(void **head, pnext next_fn, Mutex *mutex);

typedef int (*cmp_fn)(const void *w1, const void *w2);

void list_add_sorted(void **head, void *w, pnext next_fn, cmp_fn cmp, Mutex *mutex);

typedef int (*visitor)(void *w, void *arg);

void list_visit(void **head, pnext next_fn, visitor fn, void *arg, Mutex *mutex);

#endif // __LIST_H__

//  FIN
