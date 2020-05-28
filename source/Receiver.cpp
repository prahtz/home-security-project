#include "Receiver.h"

inline Receiver::Receiver() {
    this->pin = 0;
    stopReceive = false;
}

Receiver::Receiver(int pin) {
    this->pin = pin;
    stopReceive = false;
}

void Receiver::startReceiving() {
    wiringPiSetup();
    rc = RCSwitch();
    rc.enableReceive(pin);
    int buffSize = 0;
    stopReceive = false;
    while(!stopReceive) {
        if (rc.available()) {
    
            int codeRecieved = rc.getReceivedValue();
        
            if (codeRecieved == 0) {
                throw "Codifica sconosciuta";
            } else {      
                mBuff.lock();
                if(buffSize == BUFFMAX) 
                    codesBuffer.pop_back();
                codesBuffer.push_front(codeRecieved);
                mBuff.unlock();
            }
            rc.resetAvailable();
        }
        //usleep(100); 
    }
    usleep(1000000); 
}

void Receiver::stopReceiving() {
    stopReceive = true;
}

bool Receiver::isCodeAvailable() {
    return codesBuffer.size() != 0;
}

int Receiver::getLastCodeReceived() {
    return codesBuffer.front();
}