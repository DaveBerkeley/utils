
#include <stdint.h>

#include "list.h"
#include "dispatch.h"
#include "messages.h"

    /*
     *  Example dispatch library
     */

static List observers;

void dispatch_observe(Observer* obs)
{
    observers.insert(obs);
}

void dispatch_remove(Observer* obs)
{
    observers.remove(obs);
}

    /*
     *  Implement a dispatcher iter_fn for each message type.
     */

MAKE_MESSAGE_DISPATCH(XMsg);
MAKE_MESSAGE_DISPATCH(FMsg);

    /*
     *  Decode message, call dispatcher for that message
     */

static int dispatch(Message* msg, void* m, int size)
{
    if (size != msg->size())
        return -1;
    observers.iterate(m, msg->dispatcher());
    return 0;
}

int dispatch(void* m, int size)
{
    switch (*(uint8_t*)m)
    {
        case MT_X : CALL_DISPATCH(XMsg, m, size);   break;
        case MT_F : CALL_DISPATCH(FMsg, m, size);   break;
    }
    return -1;
}

//  FIN
