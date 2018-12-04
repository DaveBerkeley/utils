
#include <gtest/gtest.h>

#include "list.h"

typedef struct Item
{
    int i;
    struct Item *next;
    int j;
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
        size = list_size((void**) & head, pnext_item, 0);
        EXPECT_EQ(i+1, size);
    }

    //  Pop off stack
    for (int i = num-1; i >= 0; i--)
    {
        item = (Item*) list_pop((void**) & head, pnext_item, 0);
        EXPECT_TRUE(item);
        // check it is the correct item
        EXPECT_EQ(& items[i], item);

        size = list_size((void**) & head, pnext_item, 0);
        EXPECT_EQ(i, size);

        // check the item is correct
        EXPECT_EQ(item->i, 100 + i);
        EXPECT_EQ(item->j, 200 + i);
        EXPECT_EQ(0, item->next);
    }

    EXPECT_EQ(0, head);

    item = (Item*) list_pop((void**) & head, pnext_item, 0);
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
        size = list_size((void**) & head, pnext_item, 0);
        EXPECT_EQ(i+1, size);
    }

    //  Pop off stack
    for (int i = 0; i < num; i++)
    {
        item = (Item*) list_pop((void**) & head, pnext_item, 0);
        EXPECT_TRUE(item);
        // check it is the correct item
        EXPECT_EQ(& items[i], item);

        // check the item is correct
        EXPECT_EQ(item->i, 100 + i);
        EXPECT_EQ(item->j, 200 + i);
        EXPECT_EQ(0, item->next);
    }

    EXPECT_EQ(0, head);

    item = (Item*) list_pop((void**) & head, pnext_item, 0);
    EXPECT_EQ(0, item);

    size = list_size((void**) & head, pnext_item, 0);
    EXPECT_EQ(0, size);
}

//  FIN
