#pragma once
#include <unistd.h>
#include "Definitions.h"
#include <iostream>

class RCSwitch {
    private:
        int var = 1;
        code arrayCode[100];
    public:
        RCSwitch() {
            arrayCode[0] = 4;
            arrayCode[1] = 3;
            arrayCode[2] = 3;
            arrayCode[3] = 3;
            arrayCode[4] = 3;
        };
        void enableReceive(int PIN){}
        bool available(){return true;}
        int getReceivedValue();
        void resetAvailable() {}
        void enableTransmit(int PIN){}
        void send(unsigned long code, unsigned int bitLength) {
            std::cout<< "SEND: " << code << std::endl;
        }
        void setRepeatTransmit(int a) {

        }
};

int wiringPiSetup();