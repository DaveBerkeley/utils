
#include "callback.h"
#include "log.h"

LOGGING;

static void fn(void* arg, void *args)
{
    Callback *cb = (Callback*) arg;
    const char* text = (const char*) args;

    XLOG_DEBUG("fn(%s) %s", cb->name, text);
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
    XLOG_DEBUG("start ctor");
    callback_add(& cbs, & cb);
}

static void see_log(const LogInfo *log, void *arg)
{
    XLOG_DEBUG("log %s", log->path);
}

int main(int arg, char** argv)
{
    XLOG_DEBUG("start main");

    callback_run(& cbs, "test parameter");
    callback_remove_all(& cbs);

    XLOG_DEBUG("");

    log_visit(see_log, 0);

    return 0;
}

//  FIN
