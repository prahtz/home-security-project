#include "Transmitter.h"

Transmitter::Transmitter() {
    wiringPiSetup();
    pin = 24;
    bitLength = 24;
    transmitDelay = 100;
    rc.enableTransmit(pin);
    transmissionEnabled = false;
    transmittingCode = 0;
    stopTransmitting = false;
}

void Transmitter::startTransmitting() {
    while(!stopTransmitting) {
        cout << "START TRANSMITTING" << endl;
        unique_lock<mutex> transmitterLock(mTransmit);
        transmitCodeChanged.wait(transmitterLock, [this] {return isTransmissionEnabled();});
        while(transmissionEnabled) {   
            mTransmitCode.lock(); 
            rc.send(transmittingCode, bitLength);
            mTransmitCode.unlock();
            usleep(transmitDelay);
        } 
    }
    
    cout << "STOP TRANSMITTING" << endl;
}

void Transmitter::setTransmittingCode(code transmittingCode) {
    this->transmittingCode = transmittingCode;
}

bool Transmitter::isTransmissionEnabled() {
    return transmissionEnabled;
}
void Transmitter::isTransmissionEnabled(bool transmissionEnabled) {
    this->transmissionEnabled = transmissionEnabled;
}

