#include "PowerLoss.h"
void PowerLoss::startService() {
    call = false;
    wiringPiSetup();
    wiringPiISR(PIN, INT_EDGE_BOTH, callback);
}