
#if !defined(__LIST_H__)

#define __LIST_H__

typedef void** (*pnext)(void *item);

void list_insert(void **head, void *w, pnext next_fn, Mutex *mutex);
void list_append(void **head, void *w, pnext next_fn, Mutex *mutex);
void list_remove(void **head, void *w, pnext next_fn, Mutex *mutex);

typedef int (*cmp_fn)(const void *w1, const void *w2);

void list_add_sorted(void **head, void *w, pnext next_fn, cmp_fn cmp, Mutex *mutex);

#endif // __LIST_H__

//  FIN
