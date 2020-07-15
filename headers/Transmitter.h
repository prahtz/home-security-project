
#if defined(RPI)
    #include "RCSwitch.h"
#else
    #include "RCSim.h"
#endif

#include <unistd.h>
#include "Definitions.h"
#include "TransmitMode.h"
#include <atomic>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <list>

using namespace std;

class Transmitter {
    private:
        RCSwitch rc;
        int bitLength;
        unsigned long transmitDelay;
        atomic<bool> stopTransmitting, ackReceived;
        TransmitMode transmitMode;
        code transmittingCode;
        list<pair<code, TransmitMode>> codesBuffer;
    public:
        Transmitter();
        condition_variable startTransmitting;
        mutex mTransmit;
        bool isAckReceived();
        void isAckReceived(bool ackReceived);
        void addTransmittingCode(code transmittingCode, TransmitMode transmitMode);
        code getTransmittingCode();
        void startTransmittingProtocol();
};
