#include "RCSim.h"


int RCSwitch::getReceivedValue() {
    usleep(10000000);
    return var++;
}

int wiringPiSetup() {
    return 1;
}