
#include <map>
#include <string>

#include <stdlib.h>
#include <string.h>

#include "mutex.h"
#include "lock.h"

template <class A, class B>
class Dict
{
protected:
    Mutex   mutex;

    typedef std::map<A, B> Map;
    Map     map;

public:
 
    void add(A a, B b)
    {
        Lock lock(mutex);

        map[a] = b;
    }

    bool has(A key)
    {
        Lock lock(mutex);

        typename Map::iterator i = map.find(key);
        return i != map.end();
    }

    void rm(A key)
    {
        Lock lock(mutex);

        map.erase(key);
    }
};

    /*
     *
     */

Dict<std::string, std::string> test;
Dict<std::string, int> test2;


