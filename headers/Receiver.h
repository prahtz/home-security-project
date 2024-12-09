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
#include <chrono>
#include <thread>
#include "CriticalSection.h"
using namespace std;

#define BUFFMAX 100

class Receiver {
    protected:
        RCSwitch rc;
        atomic<bool> stopReceive;
    public:
        mutex mBuff;
        Receiver();
        void startReceiving();   
        void stopReceiving();    
        bool isBufferEmpty();
        int popCodeFromBuffer();
};
