#include "RCSim.h"


int RCSwitch::getReceivedValue() {
    usleep(3000000);
    return ++var;
    return arrayCode[var++];
}

int wiringPiSetup() {
    return 1;
}