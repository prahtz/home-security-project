class RCSim {

};

class RCSwitch {
    public:
        RCSwitch() {};
        void enableReceive(int PIN);
        bool available();
        int getReceivedValue();
        void resetAvailable();

};

int wiringPiSetup();