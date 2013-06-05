
    /*
     *  Example messages for Message Dispatch
     *
     *  Copyright (C) Dave Berkeley 2012.
     */

#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdint.h>

#include "list.h"

#include "dispatch.h"

    /*
     *  Enum of Message Types
     */

typedef enum {
    MT_X = 1,
    MT_F,
}   MsgType;

    /*
     *  Message contents
     */

#pragma pack (push,1)

typedef struct {
    uint8_t cmd; // MT_X
    int a;
    uint8_t b : 5;
    uint8_t c : 3;
}   x_t;

typedef struct {
    uint8_t cmd; // MT_F
    float a;
}   f_t;

#pragma pack (pop)

    /*
     *  Define message class for each message
     */

MAKE_MESSAGE_CLASS(XMsg, x_t);
MAKE_MESSAGE_CLASS(FMsg, f_t);

    /*
     *  Observer interface
     */

class Observer : public List
{
public:
    MAKE_MESSAGE_HANDLER(XMsg);
    MAKE_MESSAGE_HANDLER(FMsg);
};

    /*
     *  Dispatch API
     */

// Main message dispatch
int dispatch(void* m, int size);

// Subject observer add/remove
void dispatch_observe(Observer* obs);
void dispatch_remove(Observer* obs);

#endif // MESSAGES_H

//  FIN
