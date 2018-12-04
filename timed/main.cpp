
#include <stdlib.h>

#include <gtest/gtest.h>

#include "list.h"

typedef struct Item
{
    int i;
    struct Item *next;
    int j;
    bool okay;
} Item;

static void** pnext_item(void *v)
{
    Item *item = (Item*) v;
    return (void**) & item->next;
}

static void init_item(Item *item, int n)
{
    item->i = n + 100;
    item->j = n + 200;
    item->next = 0;
    item->okay = false;
}

static void item_validate(Item *item, int i)
{
    // check the item is correct
    EXPECT_EQ(item->i, 100 + i);
    EXPECT_EQ(item->j, 200 + i);
    EXPECT_EQ(0, item->next);
}

static Item * item_pop(Item **head)
{
    return (Item*) list_pop((void**) head, pnext_item, 0);
}

static int item_size(Item **head)
{
    return list_size((void**) head, pnext_item, 0);
}

static bool item_remove(Item **head, Item *w, Mutex *mutex)
{
    return list_remove((void**) head, w, pnext_item, mutex);
}

static int item_cmp(const void *w1, const void *w2)
{
    Item *i1 = (Item*) w1;
    Item *i2 = (Item*) w2;

    return i2->i - i1->i;
}


    /*
     *
     */

TEST(List, Stack)
{
    int size;
    Item *head = 0;

    size = list_size((void**) & head, pnext_item, 0);
    EXPECT_EQ(0, size);

    Item *item = 0;
    int num = 100;
    Item items[num];

    //  Initialise each item
    for (int i = 0; i < num; i++)
    {
        init_item(& items[i], i);
    }

    //  Push onto stack
    for (int i = 0; i < num; i++)
    {
        list_push((void**) & head, & items[i], pnext_item, 0);
        size = item_size(& head);
        EXPECT_EQ(i+1, size);
    }

    //  Pop off stack
    for (int i = num-1; i >= 0; i--)
    {
        item = item_pop(& head);
        EXPECT_TRUE(item);
        // check it is the correct item
        EXPECT_EQ(& items[i], item);

        size = item_size(& head);
        EXPECT_EQ(i, size);

        // check the item is correct
        item_validate(item, i);
    }

    EXPECT_EQ(0, head);

    item = item_pop(& head);
    EXPECT_EQ(0, item);
}

    /*
     *
     */

TEST(List, Append)
{
    int size;
    Item *head = 0;

    Item *item = 0;
    int num = 100;
    Item items[num];

    //  Initialise each item
    for (int i = 0; i < num; i++)
    {
        init_item(& items[i], i);
    }

    //  Append to stack
    for (int i = 0; i < num; i++)
    {
        list_append((void**) & head, & items[i], pnext_item, 0);
        size = item_size(& head);
        EXPECT_EQ(i+1, size);
    }

    //  Pop off stack
    for (int i = 0; i < num; i++)
    {
        item = item_pop(& head);
        EXPECT_TRUE(item);
        // check it is the correct item
        EXPECT_EQ(& items[i], item);

        // check the item is correct
        item_validate(item, i);
    }

    EXPECT_EQ(0, head);

    item = item_pop(& head);
    EXPECT_EQ(0, item);

    size = item_size(& head);
    EXPECT_EQ(0, size);
}

    /*
     *
     */

TEST(List, Remove)
{
    int size;
    Item *head = 0;

    //Item *item = 0;
    int num = 100;
    Item items[num];

    //  Initialise each item
    for (int i = 0; i < num; i++)
    {
        init_item(& items[i], i);
    }

    //  Append to stack
    for (int i = 0; i < num; i++)
    {
        list_append((void**) & head, & items[i], pnext_item, 0);
        size = item_size(& head);
        EXPECT_EQ(i+1, size);
    }

    size = item_size(& head);
    EXPECT_EQ(num, size);

    //  Remove last item
    bool  found;
    int i = num - 1;
    Item *item = & items[i];
    found = item_remove(& head, item, 0);
    EXPECT_EQ(true, found);
    item->okay = true;

    item_validate(item, i);
    // check it has gone
    size = item_size(& head);
    EXPECT_EQ(num-1, size);

    // can't remove it twice
    found = item_remove(& head, item, 0);
    EXPECT_EQ(false, found);

    //  Remove the first item
    i = 0;
    item = & items[i];
    found = item_remove(& head, item, 0);
    EXPECT_EQ(true, found);
    item->okay = true;

    // check it has gone
    size = item_size(& head);
    EXPECT_EQ(num-2, size);

    // can't remove it twice
    found = item_remove(& head, item, 0);
    EXPECT_EQ(false, found);

    //  Remove from the middle
    i = num / 2;
    item = & items[i];
    found = item_remove(& head, item, 0);
    EXPECT_EQ(true, found);
    item->okay = true;

    // check it has gone
    size = item_size(& head);
    EXPECT_EQ(num-3, size);

    // can't remove it twice
    found = item_remove(& head, item, 0);
    EXPECT_EQ(false, found);

    //  pop the rest off
    while (item_pop(& head))
    {
    }

    //  Try removing from an empty list
    EXPECT_EQ(0, head);
    found = item_remove(& head, item, 0);
    EXPECT_EQ(false, found);

}

    /*
     *
     */

TEST(List, Sorted)
{
    int size;
    Item *head = 0;

    Item *item = 0;
    int num = 100;
    Item items[num];

    //  Initialise each item
    for (int i = 0; i < num; i++)
    {
        init_item(& items[i], i);
    }

    // add random items sorted
    int done = 0;
    while (done < num)
    {
        long int r = random();
        int idx = r % num;

        item = & items[idx];
        if (item->okay)
        {
            continue;
        }

        item->okay = true;
        done += 1;
        list_add_sorted((void**) & head, item, pnext_item, item_cmp, 0);
        size = item_size(& head);
        EXPECT_EQ(done, size);
    }

    //  Pop off stack : it should be in order
    for (int i = 0; i < num; i++)
    {
        item = item_pop(& head);
        EXPECT_TRUE(item);
        // check it is the correct item
        EXPECT_EQ(& items[i], item);

        // check the item is correct
        item_validate(item, i);
    }

    EXPECT_EQ(0, head);

    item = item_pop(& head);
    EXPECT_EQ(0, item);

    size = item_size(& head);
    EXPECT_EQ(0, size);
}

    /*
     *
     */

typedef struct {
    Item *head;
    Mutex mutex;
}   ThreadTest;

void * thrash(void *arg)
{
    ASSERT(arg);
    ThreadTest *tt = (ThreadTest*) arg;
    Item **head = & tt->head;
    Mutex *mutex = & tt->mutex;

    Item *item = 0;
    int num = 100;
    Item items[num];

    //  Initialise each item
    for (int i = 0; i < num; i++)
    {
        init_item(& items[i], i);
    }

    // add random items sorted
    int done = 0;
    while (done < num)
    {
        long int r = random();
        int idx = r % num;

        item = & items[idx];
        if (item->okay)
        {
            continue;
        }

        item->okay = true;
        done += 1;
        list_add_sorted((void**) head, item, pnext_item, item_cmp, mutex);
    }

    //  Remove the items
    for (int i = 0; i < num; i++)
    {
        item = & items[i];
        bool found = item_remove(head, item, mutex);
        EXPECT_TRUE(found);
        // check it is the correct item
        EXPECT_EQ(& items[i], item);

        // check the item is correct
        item_validate(item, i);
    }

    return 0;
}

    /*
     *
     */

TEST(List, Thread)
{
    ThreadTest tt = { 0, MUTEX_INIT, };

    // check the test works
    thrash(& tt);
    EXPECT_EQ(0, tt.head);

    int num = 100;
    pthread_t threads[num];

    for (int i = 0; i < num; i++)
    {
        int err = pthread_create(& threads[i], 0, thrash, & tt);
        ASSERT(err == 0);
    }

    for (int i = 0; i < num; i++)
    {
        int err = pthread_join(threads[i], 0);
        ASSERT(err == 0);
    }

    // should be an empty list
    EXPECT_EQ(0, tt.head);
    int size = item_size(& tt.head);
    EXPECT_EQ(0, size);
}

//  FIN
