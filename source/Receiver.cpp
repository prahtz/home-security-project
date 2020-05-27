#include "Receiver.h"

inline Receiver::Receiver() {
    this->pin = 0;
    stopReceive = false;
}

inline Receiver::Receiver(int pin) {
    this->pin = pin;
    stopReceive = false;
}

void Receiver::startReceiving() {
    wiringPiSetup();
    rc = RCSwitch();
    rc.enableReceive(pin);
    int buffSize = 0;
    while(!stopReceive) {
        if (rc.available()) {
    
            int codeRecieved = rc.getReceivedValue();
        
            if (codeRecieved == 0) {
                throw "Codifica sconosciuta";
            } else {      
                
                if(buffSize == BUFFMAX) 
                    codesBuffer.pop_back();
                codesBuffer.push_front(codeRecieved);
            }
            rc.resetAvailable();
        }
        //usleep(100); 
    }
}