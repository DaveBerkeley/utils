
    /*
     *  Message Dispatch
     *
     *  Copyright (C) Dave Berkeley 2012.
     */

#ifndef DISPATCH_H
#define DISPATCH_H

class List;

class Message
{
public:
    typedef void (*iterator)(void*,List*);
    virtual int size() = 0;
    virtual iterator dispatcher() = 0;
};

    /*
     *  Create a MESSAGE_CLASS for each Message type
     */

#define MAKE_MESSAGE_CLASS(x,data) \
    class x : public Message \
    { \
    public: \
        typedef data Data; \
    private: \
        virtual int size() { return sizeof(Data); } \
        virtual iterator dispatcher() { return dispatch; } \
        static void dispatch(void* obj, List* item); \
    }

    /*
     *  Add handler for each message type to the Observer interface
     */

#define MAKE_MESSAGE_HANDLER(x) \
    virtual void onMsg(const x::Data& msg) {}

    /*
     *  Create a dispatch() iter_fn for each Message type
     */

#define MAKE_MESSAGE_DISPATCH(x) \
    void x::dispatch(void* obj, List* item) \
    {   \
        Observer* obs = (Observer*) item;   \
        Data* data = (Data*) obj;   \
        obs->onMsg(*data);  \
    }

    /*
     *  Call dispatch() for a specific Message type.
     *
     *  Used to build a dispatch() function 
     *  for a specific set of messages.
     */

#define CALL_DISPATCH(x,data,size) \
    { x msg; return dispatch(& msg, data, size); }

#endif // DISPATCH_H

//  FIN
