#include "RCSim.h"


int RCSwitch::getReceivedValue() {
    usleep(10000000);
    return arrayCode[var++];
}

int wiringPiSetup() {
    return 1;
}