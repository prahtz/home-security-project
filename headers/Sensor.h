#pragma once
#include "State.h"
#include "SensorTypes.h"
#include "Definitions.h"
#include "Action.h"
#include <list>
#include <fstream>
#include <iostream>

using namespace std;

class Sensor {
    private:
        int sensorID;
        State sensorState;
        bool charged;
        bool enabled;
        list<pair<code, Action>> codeActionList;
    public:
        Sensor() {};
        Sensor(int sensorID, State sensorState, bool enabled);
        int getSensorID();
        State getSensorState();
        bool isEnabled();
        bool isCharged();
        void setSensorID(int sensorID);
        void setSensorState(State state);
        void isEnabled(bool enabled);
        void isCharged(bool charged);
        bool isSensorReady();
        bool operator==(Sensor& s);
        virtual list<pair<code, Action>> getCodeActionList() = 0;
        virtual void writeToFile(ofstream &out);
        virtual string getSensorInfo();
        /*virtual bool isSensorReady();*/
};