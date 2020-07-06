#include "Transmitter.h"

Transmitter::Transmitter()
{
    wiringPiSetup();
    pin = 24;
    bitLength = 24;
    transmitDelay = 1000;
    rc.enableTransmit(pin);
    transmittingCode = 0;
    transmissionEnabled = false;
    stopTransmitting = false;
    waitForAck = true;
}

void Transmitter::startTransmittingProtocol()
{
    while (!stopTransmitting)
    {
        cout << "START TRANSMITTING" << endl;
        unique_lock<mutex> transmitterLock(mTransmit);
        startTransmitting.wait(transmitterLock, [this] { return isTransmissionEnabled();});
        while (isTransmissionEnabled() && isWaitForAck())
        {
            rc.send(transmittingCode, bitLength);
            startTransmitting.wait_for(transmitterLock, chrono::milliseconds(transmitDelay), [this] { return !isTransmissionEnabled() || !isWaitForAck(); });
        }
        if(!isWaitForAck()) {
            rc.send(transmittingCode, bitLength);
            transmissionEnabled = false;
            waitForAck = true;
        }
    }
    cout << "STOP TRANSMITTING" << endl;
}

void Transmitter::setTransmittingCode(code transmittingCode)
{
    this->transmittingCode = transmittingCode;
}

bool Transmitter::isTransmissionEnabled()
{
    return transmissionEnabled;
}
void Transmitter::isTransmissionEnabled(bool transmissionEnabled)
{
    this->transmissionEnabled = transmissionEnabled;
}

bool Transmitter::isWaitForAck()
{
    return waitForAck;
}

void Transmitter::isWaitForAck(bool waitForAck)
{
    this->waitForAck = waitForAck;
}
