
#include "Core.h"


Core::Core() : receiver(PIN), eventHandler(&receiver, &knownSensorList, &codeMap){
    this->alarmActivated = false;
    setupKnownSensors();


    receiverThread = thread(&Receiver::startReceiving, &receiver);
    eventHandlerThread = thread(&EventHandler::startListening, &eventHandler);

    registerNewDoorSensor();

    eventHandlerThread.join();
    receiverThread.join();
};


//TEST
//TODO
void Core::setupKnownSensors() {
    ifstream readingFile(KNOWN_PATH);
    string line;
    if(readingFile.is_open()) {
        while(getline(readingFile, line)) {
            istringstream streamString(line);
            string field;
            getline(streamString, field, ';');
            switch(stoi(field)) {
                case DOOR_SENSOR:
                    DoorSensor* ds = new DoorSensor();
                    getline(streamString, field, ';');
                    ds->setSensorID(stoi(field));
                    getline(streamString, field, ';');
                    ds->setSensorState((State) stoi(field));
                    getline(streamString, field, ';');
                    ds->setOpenCode(stoi(field));
                    getline(streamString, field);
                    ds->setCloseCode(stoi(field));
                    knownSensorList.push_back(ds);
                    codeMap[ds->getOpenCode()]= new pair<Action, Sensor*>(OPEN, ds);
                    codeMap[ds->getCloseCode()] = new pair<Action, Sensor*>(CLOSE, ds);
                    break;
            }       
        }
        readingFile.close();
    }
    else {
        ofstream createdFile(KNOWN_PATH);
        createdFile.close();
    }
    
};
//TEST
//Return true if sensor added, false otherwise
bool Core::addSensorToList(Sensor* s, list<Sensor*>* sensorList) {
    list<Sensor*>::iterator it = std::find_if(sensorList->begin(), sensorList->end(), [s](Sensor* sensor) {return sensor->getSensorID() == s->getSensorID();});
    if(it == sensorList->end()) {
        sensorList->push_back(s);
        return true;
    }
    return false;
};
//TEST
//Return true if sensor removed, false otherwise
bool Core::removeSensorFromList(Sensor* s, list<Sensor*>* sensorList) {
    int listSize = sensorList->size();
    sensorList->remove_if([s](Sensor* sensor) {return sensor->getSensorID() == s->getSensorID();});
    if(listSize != sensorList->size()) 
        return true;
    return false;
};

//TEST
//Return true if alarm is activatable, false otherwise
bool Core::isAlarmReady(AlarmType at) {
    switch(at) {
        case COMPLETE:
            list<Sensor*>::iterator it = std::find_if(activeSensorList.begin(), activeSensorList.end(), [](Sensor* s) {return !s->isSensorReady();});
            if(it == activeSensorList.end())
                return true;
            break;
    }
    return false;
}

//??
int Core::getNewSensorID() {
    if(knownSensorList.empty())
        return 0;
    return (*(--knownSensorList.end()))->getSensorID() + 1;
}

//TO TEST
void Core::registerNewDoorSensor() {
    DoorSensor* ds = new DoorSensor();
    ds->setSensorID(getNewSensorID());
    ds->setSensorState(OPENED);

    eventHandler.registerCode = true;
    unique_lock<mutex> registerLock(eventHandler.mNewCode);
    eventHandler.newCodeAvailable.wait(registerLock, [this] {return (bool) eventHandler.codeArrived;});

    eventHandler.codeArrived = false;
    code closeCode = eventHandler.newCode;
    ds->setCloseCode(closeCode);
    cout<<"Core - closeCode: "<< closeCode << endl;

    bool go = true;
    while(go) {
        eventHandler.newCodeAvailable.wait(registerLock, [this] {return (bool) eventHandler.codeArrived;});
        eventHandler.codeArrived = false;
        code openCode = eventHandler.newCode;
        if(openCode != closeCode) {
            ds->setOpenCode(openCode);
            cout<<"Core - openCode: "<< openCode << endl;
            go = false;
        }
    }
    eventHandler.registerCode = false;
    
    eventHandler.mSensorList.lock();
    addSensorToList(ds, &knownSensorList);
    eventHandler.mSensorList.unlock();

    eventHandler.mFile.lock();
    ofstream out(KNOWN_PATH, ios::app);
    ds->writeToFile(out);
    out.close();
    eventHandler.mFile.unlock();
    cout<<"Core - FINE"<<endl;
}


