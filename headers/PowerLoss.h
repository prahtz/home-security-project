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

#include "FirebaseMessagesHandler.h"
#include "FirebaseNotification.h"
#include "Core.h"


static std::atomic<bool> call;
static const int PIN = 22;
static const int LOW_VALUE = 1;
static void callback() {
    unsigned int sleep = 10000;
    int r = digitalRead(PIN);
    if(r == LOW_VALUE) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        r = digitalRead(PIN);
        if(r == LOW_VALUE) {
            for(string token : Core::tokenList) {
                std::cout << token << std::endl;
                FirebaseNotification* notification = new FirebaseNotification();
                notification->setTitle("CORRENTE ASSENTE!");
                notification->setBody("Rilevata assenza di corrente presso la centralina.");
                notification->setToken(token);
                notification->setTTL("18000");
                notification->setNType("battery");
                FirebaseMessagesHandler::addMessage(notification);
                Logger::log("Power loss detected");
                statical::newFirebaseNotification.notify_all();
            }
        }
    }
}

class PowerLoss {
    private:
    public:
        PowerLoss(){};
        static void startService();
};