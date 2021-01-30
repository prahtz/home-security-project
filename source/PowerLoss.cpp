#include "PowerLoss.h"

void PowerLoss::test() {
    call = false;
    wiringPiISR(PIN, INT_EDGE_BOTH,[] () {call = call == true ? false : true;});
}

void PowerLoss::callback() {

}