#include "RCSim.h"

int var = 1;

int RCSwitch::getReceivedValue() {
    usleep(3000000);
    int result = var;
    if(var == 5)
        var = 1;
    else
        var++;
    return result;
}

int wiringPiSetup() {
    return 1;
}