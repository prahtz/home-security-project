#include "PowerLoss.h"
void PowerLoss::test() {
    call = false;
    wiringPiSetup();
    wiringPiISR(PIN, INT_EDGE_BOTH, callback);
}