#pragma once
#include <unistd.h>
#include "Definitions.h"
#include <iostream>

class RCSwitch {
    private:
        int var = 0;
        code arrayCode[100];
    public:
        RCSwitch() {
            arrayCode[0] = 1;
            arrayCode[1] = 2;
            arrayCode[2] = ackSirenCode;
            arrayCode[3] = 88;
            arrayCode[4] = ackSirenCode;
        };
        void enableReceive(int PIN){}
        bool available(){return true;}
        int getReceivedValue();
        void resetAvailable() {}
        void enableTransmit(int PIN){}
        void send(unsigned long code, unsigned int bitLength) {
            std::cout<< code << std::endl;
        }
};

int wiringPiSetup();