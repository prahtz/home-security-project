#include <unistd.h>
class RCSwitch {
    public:
        RCSwitch() {};
        void enableReceive(int PIN){}
        bool available(){return true;}
        int getReceivedValue();
        void resetAvailable() {}

};

int wiringPiSetup();