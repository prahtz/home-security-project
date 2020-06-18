
#if defined(RPI)
    #include "RCSwitch.h"
#else
    #include "RCSim.h"
#endif

#include "Sensor.h"
#include <atomic>

class Transmitter {
    private:
        RCSwitch rc;
        int pin;
        int bitLength;
        useconds_t transmitDelay;
        atomic<bool> transmissionEnabled;
    public:
        Transmitter();
        void startTransmitting(code code);
};