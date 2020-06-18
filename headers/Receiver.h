
#if defined(RPI)
    #include "RCSwitch.h"
#else
    #include "RCSim.h"
#endif

#define BUFFMAX 100
#include "Sensor.h"
#include <list>
#include <condition_variable>
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
    public:
        condition_variable codeAvailable;
        mutex mBuff;
        Receiver(int pin);
        void startReceiving();   
        void stopReceiving();    
        bool isBufferEmpty();
        int popCodeFromBuffer();
};
