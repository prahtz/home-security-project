#pragma once
#ifdef RPI
    #include <wiringPi.h>
#else
    #include "RCSim.h"
    const int INT_EDGE_BOTH = 0;
    static const int wiringPiISR (int pin, int edgeType,  void (*function)(void)) {return 0;}
#endif

#include <atomic>
#include <iostream>

static std::atomic<bool> call;

class PowerLoss {
    private:
        static const int PIN = 22;
    public:
        PowerLoss(){};
        static void test();
        void callback();
};