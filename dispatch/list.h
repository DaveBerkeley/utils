
    /*
     *  List utils
     *
     *  Copyright (C) Dave Berkeley 2012.
     *
     *  Used with permission.
     */

#if ! defined(_LIST_H_)
#define _LIST_H_

class List
{
public:
    List* next;
    List() : next(0) { }
    virtual ~List() { }

    static void insert(List** head, List* item)
    {
        item->next = *head;
        *head = item;
    }

    static void insert(List** head, List* item, int(*cmp_fn)(List* a, List* b))
    {
        for (; *head; head = &((*head)->next))
           if (cmp_fn(item, *head) >= 0)
              break;
        item->next = *head;
        *head = item;
    }

    static void append(List** head, List* item)
    {
        for (; *head; head = &((*head)->next))
            ;
        insert(head, item);
    }

    static List* pop(List** head)
    {
        List* item = *head;
        if (!item)
            return 0;
        *head = item->next;
        return item;
    }

    static void iterate(List* list, void* obj, void (*fn)(void*,List*))
    {
        for (; list; list = list->next)
            fn(obj, list);
    }

    static uint8_t len(List* list)
    { 
       uint8_t total = 0; 
       for (; list; list = list->next)
          total += 1;
       return total;
    }

    static List* find(List* list, void* obj, bool (*fn)(void*,List*))
    {
        for (; list; list = list->next)
            if (fn(obj, list))
                return list;
        return 0;
    }

    static List* find_remove(List** head, void* obj, bool (*fn)(void*,List*))
    {
        for (; *head; head = &((*head)->next))
            if (fn(obj, *head)) 
                return pop(head);
        return 0;
    
    }

    static bool remove(List** head, List* item)
    {
        for (; *head; head = &((*head)->next))
           if (*head == item)
              return (pop(head) ? true : false);
        return false;
    }

    void insert(List* item) { insert(& next, item); }
    void insert(List* item, int(*cmp_fn)(List* a, List* b)) { insert(& next, item, cmp_fn); }
    List* pop() { return pop(& next); }
    void append(List* item) { append(& next, item); }
    void iterate(void* obj, void (*fn)(void*, List*)) { iterate(next, obj, fn); }
    List* find(void* obj, bool(*fn)(void*, List*)) { return find(next, obj, fn); }
    List* find_remove(void* obj, bool(*fn)(void*, List*)) { return find_remove(& next, obj, fn); }
    bool remove(List* item) { return remove(& next, item); }
    List* head() { return next; }
    uint8_t len() { return len(next); }
};

#endif // _LIST_H_

// FIN
