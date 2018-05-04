
#include <stdio.h>

typedef struct Callback {
    struct Callback *next;
    void (*fn)(void* arg, void* args);
    void *arg;
    const char *name;
}   Callback;

typedef struct {
    Callback *callbacks;
    // mutexs ...
}   Callbacks;

static Callbacks cbs;

static void _callback_lock(Callbacks *cbs)
{
    //  TODO
}

static void _callback_unlock(Callbacks *cbs)
{
    //  TODO
}

static void callback_run(Callbacks *cbs, void *args)
{
    _callback_lock(cbs);

    for (Callback *cb = cbs->callbacks; cb; cb = cb->next)
    {
        printf("run %s\n", cb->name);
        cb->fn(cb->arg, args);
    }

    _callback_unlock(cbs);
}
static void callback_add(Callbacks *cbs, Callback *cb)
{
    _callback_lock(cbs);

    printf("add %p %p %s\n", cb->fn, cb->arg, cb->name);
    cb->next = cbs->callbacks;
    cbs->callbacks = cb;

    _callback_unlock(cbs);
}

static void callback_remove(Callbacks *cbs, Callback *cb)
{
    _callback_lock(cbs);

    for (Callback **head = & cbs->callbacks; *head; head = &((*head)->next))
    {
        if (cb == *head)
        {
            *head = cb->next;
            cb->next = 0;
            break;
        }
    }

    _callback_unlock(cbs);
}

static void callback_remove_all(Callbacks *cbs)
{
    while (cbs->callbacks)
    {
        callback_remove(cbs, cbs->callbacks);
    }
}

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
