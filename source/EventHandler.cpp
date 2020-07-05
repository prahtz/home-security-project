#include "EventHandler.h"

EventHandler::EventHandler(Receiver* receiver, Transmitter* transmitter, list<Sensor*>* knownSensorList, map<code, pair<Action, Sensor*>*>* codeMap) {
    this->receiver = receiver;
    this->transmitter = transmitter;
    this->knownSensorList = knownSensorList;
    this->codeMap = codeMap;

    registerCode = false;
    codeArrived = false;
    alarmActivated = false;
}

//TO TEST
void EventHandler::startListening() {
    while(true) {

        unique_lock<mutex> receiverLock(receiver->mBuff);   
        //usleep(1000000);     
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
                newCode = codeReceived;
                cout<<"EventHandler - newCode: "<< newCode << endl;
                codeArrived = true;
                newCodeAvailable.notify_all();
            }
        }
    }
    
}

//TO TEST
void EventHandler::onSensorOpen(Sensor* sensor) {
    mSensorList.lock();
    if(alarmActivated && sensor->isEnabled())
        activateDefenses();
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

void EventHandler::activateDefenses() {
   transmitter->setTransmittingCode(activateSirenCode);
   transmitter->isTransmissionEnabled(true);
   transmitter->transmitCodeChanged.notify_all();
}