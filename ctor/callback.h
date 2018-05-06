
#if ! defined(CALLBACK_H)

#define CALLBACK_H

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

void callback_run(Callbacks *cbs, void *args);
void callback_add(Callbacks *cbs, Callback *cb);
void callback_remove(Callbacks *cbs, Callback *cb);
void callback_remove_all(Callbacks *cbs);

#endif // CALLBACK_H

//  FIN
