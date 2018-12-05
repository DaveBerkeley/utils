
#include <stdlib.h>

#include <gtest/gtest.h>

#include "list.h"

    /**
     * @brief helper function to create a table of indexes
     */

static void rand_array(int *data, int num)
{
    memset(data, 0, num * sizeof(data[0]));

    //  create an ordered array
    for (int i = 0; i < num; i++)
    {
        data[i] = i;
    }

    for (int len = num; len; len--)
    {
        // pick a random element
        int r = random();
        int idx = r % len;
        // swap this idx with the end one
        int tmp = data[len-1];
        data[len-1] = data[idx];
        data[idx] = tmp;
        // each pass shrinks the array we work on ..
    }
}

TEST(Random, Array)
{
    int num = 100;
    int block[num];

    rand_array(block, num);

    bool test[num];
    memset(test, 0, sizeof(test));

    //  mark each idx in the block
    for (int i = 0; i < num; i++)
    {
        int idx = block[i];
        // each index is in the range 0 <= n < num
        EXPECT_TRUE(idx >= 0);
        EXPECT_TRUE(idx < num);
        // each idx must only appear once
        EXPECT_FALSE(test[idx]);
        test[idx] = true;
    }

    //  check that all the values are set
    for (int i = 0; i < num; i++)
    {
        EXPECT_TRUE(test[i]);
    }
}

    /*
     *
     */

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

static void item_add_sorted(Item **head, Item *w, Mutex *mutex)
{
    list_add_sorted((void**) head, w, pnext_item, item_cmp, mutex);
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

    int block[num];
    rand_array(block, num);

    // add our items randomly to sorted list
    for (int i = 0; i < num; i++)
    {
        int idx = block[i];
        item = & items[idx];
        item_add_sorted(& head, item, 0);
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

    //  create a randomised table of indexes
    int block[num];
    rand_array(block, num);

    // add our items randomly to sorted list
    for (int i = 0; i < num; i++)
    {
        int idx = block[i];
        item = & items[idx];
        item_add_sorted(head, item, mutex);
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

    /*
     *
     */

static int item_match(void *w, void *arg)
{
    Item *item = (Item*) w;
    Item *match = (Item*) arg;

    return (item->i == match->i) && (item->j == match->j);
}

TEST(List, Find)
{
    int idx;
    Item *head = 0;

    Item *item = 0;
    int num = 10;
    Item items[num];

    //  Initialise each item
    for (int i = 0; i < num; i++)
    {
        init_item(& items[i], i);
    }

    //  create a randomised table of indexes
    int block[num];
    rand_array(block, num);

    // add our items randomly to sorted list
    for (int i = 0; i < num; i++)
    {
        int idx = block[i];
        item = & items[idx];
        item_add_sorted(& head, item, 0);
    }

    //  find first item
    idx = 0;
    item = (Item*) list_find((void**) & head, pnext_item, item_match, & items[idx], 0);
    EXPECT_EQ(item, & items[idx]);

    //  find last item
    idx = num-1;
    item = (Item*) list_find((void**) & head, pnext_item, item_match, & items[idx], 0);
    EXPECT_EQ(item, & items[idx]);

    //  find middle item
    idx = num / 2;
    item = (Item*) list_find((void**) & head, pnext_item, item_match, & items[idx], 0);
    EXPECT_EQ(item, & items[idx]);

    //  try to find non-existent item
    Item wrong;
    init_item(& wrong, -5);

    item = (Item*) list_find((void**) & head, pnext_item, item_match, & wrong, 0);
    EXPECT_EQ(0, item);
}

//  FIN
