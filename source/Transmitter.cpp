#include "Transmitter.h"

Transmitter::Transmitter() {
    wiringPiSetup();
    pin = 28;
    bitLength = 24;
    transmitDelay = 100000;
    rc.enableTransmit(pin);
    transmissionEnabled = false;
}

void Transmitter::startTransmitting(code code) {
    while(transmissionEnabled) {
        rc.send(code, bitLength);
        usleep(transmitDelay);
    } 
}

