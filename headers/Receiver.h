#pragma once
#if defined(RPI)
    #include "RCSwitch.h"
#else
    #include "RCSim.h"
#endif
#include "Definitions.h"
#include <list>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <iostream>
#include <unistd.h>
using namespace std;

#define BUFFMAX 100

class Receiver {
    private:
        RCSwitch rc;
        atomic<bool> stopReceive;
        list<code> codesBuffer;
    public:
        condition_variable codeAvailable;
        mutex mBuff;
        Receiver();
        void startReceiving();   
        void stopReceiving();    
        bool isBufferEmpty();
        int popCodeFromBuffer();
};
