#include <unistd.h>
class RCSwitch {
    private:
        int var = 1;
    public:
        RCSwitch() {};
        void enableReceive(int PIN){}
        bool available(){return true;}
        int getReceivedValue();
        void resetAvailable() {}
};

int wiringPiSetup();