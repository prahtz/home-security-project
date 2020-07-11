#include "RCSim.h"


int RCSwitch::getReceivedValue() {
    usleep(3000000);
    if(var == 1)
        var = 2;
    else 
        var = 1;
    return var;
    return arrayCode[var++];
}

int wiringPiSetup() {
    return 1;
}