
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

using namespace std;

class Transmitter {
    private:
        RCSwitch rc;
        int pin;
        int bitLength;
        useconds_t transmitDelay;
        code transmittingCode;
        mutex mTransmit;
        atomic<bool> transmissionEnabled, stopTransmitting;
    public:
        Transmitter();
        condition_variable transmitCodeChanged;
        mutex mTransmitCode;
        bool isTransmissionEnabled();
        void isTransmissionEnabled(bool transmissionEnabled);
        void setTransmittingCode(code transmittingCode);
        void startTransmitting();
};
