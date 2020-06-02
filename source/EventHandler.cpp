#include "EventHandler.h"

EventHandler::EventHandler(Receiver* receiver, list<Sensor*>* knownSensorList, map<code, pair<Action, Sensor*>*>* codeMap) {
    this->receiver = receiver;
    this->knownSensorList = knownSensorList;
    this->codeMap = codeMap;
    registerCode = false;
    codeArrived = false;
}

//TO TEST
void EventHandler::startListening() {
    while(true) {
        unique_lock<mutex> receiverLock(receiver->mBuff);
        receiver->codeAvailable.wait(receiverLock, [this] {return !receiver->isBufferEmpty();});

        code codeReceived = receiver->popCodeFromBuffer();
        map<code, pair<Action, Sensor*>*>::iterator mapIterator = codeMap->find(codeReceived);
        bool knownCode = codeMap->end() != mapIterator;
        if(knownCode) {
            Action action = mapIterator->second->first;
            Sensor* sensor = mapIterator->second->second;
            switch(action) {
                case OPEN:
                    onSensorOpen(sensor);
                    break;
                case CLOSE:
                    onSensorClose(sensor);
            }
        }
        else {
            //cout<<"NO " << codeReceived<<endl;
            cout<<"EventHandler - codeReceived: "<< codeReceived << endl;
            cout<<"EventHandler - registerCode?: "<< registerCode << endl;
            if(registerCode) {
                unique_lock<mutex> registerLock(mNewCode);
                newCode = codeReceived;
                cout<<"EventHandler - newCode: "<< newCode << endl;
                codeArrived = true;
                registerLock.unlock();
                newCodeAvailable.notify_all();
            }
        }
    }
    
}

//TO TEST
void EventHandler::onSensorOpen(Sensor* sensor) {
    mSensorList.lock();
    sensor->setSensorState(OPENED);
    updateKnownFile(); 
    mSensorList.unlock();
}

//TO TEST
void EventHandler::onSensorClose(Sensor* sensor) {
    mSensorList.lock();
    sensor->setSensorState(CLOSED);
    updateKnownFile(); 
    mSensorList.unlock();
}

void EventHandler::updateKnownFile() {
    mFile.lock();
    ofstream out(KNOWN_PATH, ios::trunc);
    for(Sensor* s : (*knownSensorList))
        s->writeToFile(out);
    out.close();
    mFile.unlock();
}