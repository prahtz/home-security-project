#include "PowerLoss.h"

void PowerLoss::test() {
    call = false;
    wiringPiISR(PIN, INT_EDGE_BOTH,[] () {std::cout<<"testing"<<std::endl;});
}

void PowerLoss::callback() {

}