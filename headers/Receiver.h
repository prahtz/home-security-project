
#if defined(RPI)
    #include "RCSwitch.h"
#else
    #include "RCSim.h"
#endif

#define BUFFMAX 100

#include <list>
#include <mutex>
#include <atomic>
#include <unistd.h>
using namespace std;

class Receiver {
    private:
        int pin;
        RCSwitch rc;
        atomic<bool> stopReceive;
        list<int> codesBuffer;
        mutex mBuff;
    public:
        Receiver();
        Receiver(int pin);
        void startReceiving();   
        void stopReceiving();    
        bool isCodeAvailable();
        int getLastCodeReceived();
};
