
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

TEST(List, Stack)
{
    int size;
    Item *head = 0;

    size = list_size((void**) & head, pnext_item, 0);
    EXPECT_EQ(0, size);

    Item *item = 0;
    int num = 100;
    Item items[num];
    memset(& items, 0, sizeof(items));

    //  Identify each item
    for (int i = 0; i < num; i++)
    {
        Item *item = & items[i];
        item->i = 100 + i;
        item->j = 200 + i;
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
        EXPECT_EQ(& items[i], item);

        size = list_size((void**) & head, pnext_item, 0);
        EXPECT_EQ(i, size);

        EXPECT_EQ(item->i, 100 + i);
        EXPECT_EQ(item->j, 200 + i);
        EXPECT_EQ(0, item->next);
    }

    EXPECT_EQ(0, head);

    item = (Item*) list_pop((void**) & head, pnext_item, 0);
    EXPECT_EQ(0, item);
}

//  FIN
