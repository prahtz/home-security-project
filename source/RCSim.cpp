#include "RCSim.h"


int RCSwitch::getReceivedValue() {
    usleep(10000000);
    return ++var;
    return arrayCode[var++];
}

int wiringPiSetup() {
    return 1;
}