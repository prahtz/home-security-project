#include "EventHandler.h"

EventHandler::EventHandler(Receiver* receiver) {
    this->receiver = receiver;
}

//TESTED
Sensor* EventHandler::getSensorByCode(int codeReceived, bool* knownCode) {
    list<Sensor*>::iterator it = std::find_if(knownSensorList->begin(), knownSensorList->end(), [&codeReceived](Sensor* s) {
        list<int> codeList = s->getCodeList();
        list<int>::iterator it = std::find_if(codeList.begin(), codeList.end(), [&codeReceived](int sensorCode) {return codeReceived == sensorCode;});
        return it != codeList.end();
    });
    if(it != knownSensorList->end()) {
        *knownCode = true;
        return *it;
    }
    *knownCode = false;
    return *it;

}

//TO TEST
void EventHandler::startListening(list<Sensor*> &knownSensorList) {
    this->knownSensorList = &knownSensorList;
    while(true) {
        unique_lock<mutex> lock(receiver->mBuff);
        receiver->codeAvailable.wait(lock, [this] {return !receiver->isBufferEmpty();});
        int codeReceived = receiver->popCodeFromBuffer();
        bool knownCode = false;
        Sensor* sensor = getSensorByCode(codeReceived, &knownCode);
        if(knownCode) {
            //testa qua
            cout<<"SI " << sensor->getSensorID()<<" " << codeReceived <<endl;
        }
        else {
            cout<<"NO " << codeReceived<<endl;
        }
    }
    
}