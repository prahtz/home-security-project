#include "RCSim.h"


int RCSwitch::getReceivedValue() {
    usleep(1000000);
    return var++;
}

int wiringPiSetup() {
    return 1;
}