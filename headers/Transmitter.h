
#if defined(RPI)
    #include "RCSwitch.h"
#else
    #include "RCSim.h"
#endif

#include "Definitions.h"
#include <atomic>
#include <mutex>
#include <iostream> //FOR COUT

using namespace std;

class Transmitter {
    private:
        RCSwitch rc;
        int pin;
        int bitLength;
        useconds_t transmitDelay;
    public:
        Transmitter();

        atomic<bool> transmissionEnabled;
        
        void startTransmitting(code code);
};