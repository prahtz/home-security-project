#pragma once
#include <unistd.h>
class RCSwitch {
    private:
        int var = 1;
    public:
        RCSwitch() {};
        void enableReceive(int PIN){}
        bool available(){return true;}
        int getReceivedValue();
        void resetAvailable() {}
        void enableTransmit(int PIN){}
        void send(unsigned long code, unsigned int bitLength) {}
};

int wiringPiSetup();