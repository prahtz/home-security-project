#pragma once
#ifdef RPI
    #include <wiringPi.h>
#else
    #include "RCSim.h"
    const int INT_EDGE_BOTH = 0;
    const int INT_EDGE_RISING = 1;
    static const int digitalRead(int pin) {return 1;}
    static const int wiringPiISR (int pin, int edgeType,  void (*function)(void)) {return 0;}
#endif

#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>

#include "FirebaseMessagesHandler.h"
#include "FirebaseNotification.h"
#include "Core.h"

class PowerLoss {
    private:
        static const int PIN = 22;
        static const int LOW_VALUE = 1;
        static void callback();
        static void sendNotifications();
    public:
        PowerLoss(){};
        static void startService();
};