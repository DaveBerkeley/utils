
    /*
     *  Message Dispatch
     *
     *  Copyright (C) Dave Berkeley 2012.
     */

#include "stdio.h"

#include "messages.h"

    /*
     *  Tester classes
     */

class IntObs : public Observer
{
    virtual void onMsg(const XMsg::Data& msg)
    {
        printf("%d\n", msg.a);
    }
};

class HexObs : public Observer
{
    virtual void onMsg(const XMsg::Data& msg)
    {
        printf("%#x\n", msg.a);
    }
    virtual void onMsg(const FMsg::Data& msg)
    {
        printf("%f\n", msg.a);
    }
};

int main()
{
    IntObs obs;
    HexObs hexobs;

    dispatch_observe(& obs);
    dispatch_observe(& hexobs);

    x_t x = { MT_X, 1234 };
    f_t f = { MT_F, 1.234 };
    
    printf("==== x ====\n");
    dispatch(& x, sizeof(x));
    printf("==== f ====\n");
    dispatch(& f, sizeof(f));

    dispatch_remove(& obs);
    printf("==== x ====\n");
    dispatch(& x, sizeof(x));

    return 0;
}

// FIN
