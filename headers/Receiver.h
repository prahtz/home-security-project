
#if defined(RPI)
    #include "RCSwitch.h"
#else
    #include "RCSim.h"
#endif

#include "Definitions.h"

#define BUFFMAX 100
#include <list>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <iostream>
#include <unistd.h>
using namespace std;


class Receiver {
    private:
        int pin;
        RCSwitch rc;
        atomic<bool> stopReceive;
        list<code> codesBuffer;
    public:
        condition_variable codeAvailable;
        mutex mBuff;
        Receiver(int pin);
        void startReceiving();   
        void stopReceiving();    
        bool isBufferEmpty();
        int popCodeFromBuffer();
};
