#if defined(RPI)
    #include <RCSwitch.h>
#else
    #include "RCSim.h"
#endif

#define BUFFMAX 100

#include <list>
#include <thread>
#include <mutex>
using namespace std;


class Receiver {
    private:
        int pin;
        RCSwitch rc;
        bool stopReceive;
        list<int> codesBuffer;
    public:
        Receiver();
        Receiver(int pin);
        void startReceiving();       
};
