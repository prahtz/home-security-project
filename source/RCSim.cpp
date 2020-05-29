#include "RCSim.h"


int RCSwitch::getReceivedValue() {
    usleep(300000);
    return var++;
}

int wiringPiSetup() {
    return 1;
}