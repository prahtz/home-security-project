#pragma once
#ifdef RPI
    #include <wiringPi.h>
#else
    #include "RCSim.h"
    const int INT_EDGE_BOTH = 0;
    static const int digitalRead(int pin) {return 0;}
    static const int wiringPiISR (int pin, int edgeType,  void (*function)(void)) {return 0;}
#endif

#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>


static std::atomic<bool> call;
static const int PIN = 22;
static const int LOW_VALUE = 1;
static void callback() {
    unsigned int sleep = 3000;
    int r = digitalRead(PIN);
    if(r == LOW_VALUE) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        r = digitalRead(PIN);
        if(r == LOW_VALUE) {
            std::cout << "testing" << std::endl;
        }
    }
}

class PowerLoss {
    private:
    public:
        PowerLoss(){};
        static void test();
};