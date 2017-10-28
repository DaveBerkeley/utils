
class Mutex;

class Lock
{
private:
    Mutex& mutex;

public:
    Lock(Mutex& m);
    ~Lock();
};

