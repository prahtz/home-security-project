
#include "Core.h"


Core::Core() : receiver(PIN), eventHandler(&receiver){
    this->alarmActivated = false;
    setupKnownSensors();
    receiverThread = thread(&Receiver::startReceiving, &receiver);
    eventHandlerThread = thread(&EventHandler::startListening, &eventHandler, ref(knownSensorList));
    eventHandlerThread.join();
    receiverThread.join();
};
//TEST
//TODO
void Core::setupKnownSensors() {
    ifstream readingFile("./csv/known.csv");
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
                    break;
            }       
        }
    }
    else
        ofstream createdFile("./csv/known.csv");
};
//TEST
//Return true if sensor added, false otherwise
bool Core::addSensorToList(Sensor* s, list<Sensor*> sensorList) {
    list<Sensor*>::iterator it = std::find_if(sensorList.begin(), sensorList.end(), [s](Sensor* sensor) {return sensor->getSensorID() == s->getSensorID();});
    if(it == sensorList.end()) {
        sensorList.push_back(s);
        return true;
    }
    return false;
};
//TEST
//Return true if sensor removed, false otherwise
bool Core::removeSensorFromList(Sensor* s, list<Sensor*> sensorList) {
    int listSize = sensorList.size();
    sensorList.remove_if([s](Sensor* sensor) {return sensor->getSensorID() == s->getSensorID();});
    if(listSize != sensorList.size()) 
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
    return (*knownSensorList.end())->getSensorID() + 1;
}

