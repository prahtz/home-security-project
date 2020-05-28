#include "RCSim.h"


int RCSwitch::getReceivedValue() {
    usleep(3000000);
    return 1234;
}

int wiringPiSetup() {
    return 1;
}