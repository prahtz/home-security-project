
#if defined(RPI)
    #include "RCSwitch.h"
#else
    #include "RCSim.h"
#endif

#include <unistd.h>
#include "Definitions.h"
#include <atomic>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <thread>

using namespace std;

class Transmitter {
    private:
        RCSwitch rc;
        int pin;
        int bitLength;
        unsigned long transmitDelay;
        code transmittingCode;
        atomic<bool> transmissionEnabled, stopTransmitting, ackReceived;
    public:
        Transmitter();
        condition_variable startTransmitting;
        mutex mTransmit;
        bool isTransmissionEnabled();
        void isTransmissionEnabled(bool transmissionEnabled);
        void setTransmittingCode(code transmittingCode);
        void startTransmittingProtocol();
};
