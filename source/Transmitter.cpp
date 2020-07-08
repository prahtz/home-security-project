#include "Transmitter.h"

Transmitter::Transmitter()
{
    wiringPiSetup();
    pin = 24;
    bitLength = 24;
    transmitDelay = 1000;
    rc.enableTransmit(pin);
    transmittingCode = 0;
    transmitMode = WAIT_FOR_ACK;
    ackReceived = false;
    stopTransmitting = false;
}

void Transmitter::startTransmittingProtocol()
{
    while (!stopTransmitting)
    { 
        unique_lock<mutex> transmitterLock(mTransmit);
        startTransmitting.wait(transmitterLock, [this] { return !codesBuffer.empty();});
        transmittingCode = codesBuffer.back().first;
        transmitMode = codesBuffer.back().second;
        codesBuffer.pop_back();
        switch(transmitMode) {
            case WAIT_FOR_ACK:
                while (!isAckReceived())
                {
                    rc.send(transmittingCode, bitLength);
                    startTransmitting.wait_for(transmitterLock, chrono::milliseconds(transmitDelay), [this] { return isAckReceived(); });
                }
                transmittingCode = 0;
                isAckReceived(false);
                break;
            case SEND_ACK:
                rc.send(transmittingCode, bitLength);
                rc.send(transmittingCode, bitLength);
                break;
        }
        
    }
}

void Transmitter::addTransmittingCode(code transmittingCode, TransmitMode transmitMode)
{
    pair<code, TransmitMode> transmit(transmittingCode, transmitMode);
    codesBuffer.push_front(transmit);
}

code Transmitter::getTransmittingCode() {
    return transmittingCode;
}

bool Transmitter::isAckReceived()
{
    return ackReceived;
}
void Transmitter::isAckReceived(bool ackReceived)
{
    this->ackReceived = ackReceived;
}
