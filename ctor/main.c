
#include <stdio.h>

#include "callback.h"

#define __STRINGISE(x) #x
#define STRINGISE(x) __STRINGISE(x)

static void fn(void* arg, void *args)
{
    Callback *cb = (Callback*) arg;
    const char* text = (const char*) args;

    printf("fn(%s) %s\n", cb->name, text);
}

static Callback cb = {
    .fn = fn,
    .arg = & cb,
    .name = STRINGISE(__CWD__) "/" __FILE__,
};


static Callbacks cbs;

__attribute__((constructor))
static void test()
{
    printf("start ctor\n");
    callback_add(& cbs, & cb);
}

int main(int arg, char** argv)
{
    printf("start main\n");

    callback_run(& cbs, "test parameter");
    callback_remove_all(& cbs);
    return 0;
}

//  FIN
