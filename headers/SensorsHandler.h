#pragma once

#include <string>
#include <list>
#include <fstream>
#include <filesystem>
#include <map>
#include <numeric>
#include <algorithm>

#include "Sensor.h"
#include "Definitions.h"
#include "DoorSensor.h"
#include "Action.h"

using namespace std;

class SensorsHandler
{
private:
    list<Sensor*> sensorList;
    map<code, pair<Action, Sensor*>*> codeMap;

public:
    void updateCodeMap(Sensor* s);
    void setupKnownSensors();
    void updateKnownFile();
    bool addSensorToList(Sensor* s);
    bool removeSensorFromList(int sensorID);
    bool allSensorsReady();
    int getNewSensorID();
    list<Sensor*>& getSensorList();
    map<code, pair<Action, Sensor*>*>& getCodeMap();
};