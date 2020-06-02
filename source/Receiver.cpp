#include "Receiver.h"

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
                unique_lock<mutex> lock(mBuff);
                if(codesBuffer.size() >= BUFFMAX) 
                    codesBuffer.pop_back();
                codesBuffer.push_front(codeRecieved);
                lock.unlock();
                codeAvailable.notify_all();
            }
            rc.resetAvailable();
        }
    }
}

void Receiver::stopReceiving() {
    stopReceive = true;
}

bool Receiver::isBufferEmpty() {
    return codesBuffer.empty();
}

int Receiver::popCodeFromBuffer() {
    int code = codesBuffer.back();
    codesBuffer.pop_back();
    return code;
}