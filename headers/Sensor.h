#include "State.h"
#include "SensorTypes.h"
#include "Definitions.h"
#include <list>
#include <fstream>
#include <iostream>

using namespace std;

class Sensor {
    private:
        int sensorID;
        State sensorState;
        bool enabled;
    public:
        Sensor() {};
        Sensor(int sensorID, State sensorState, bool enabled);
        int getSensorID();
        State getSensorState();
        bool isEnabled();
        void setSensorID(int sensorID);
        void setSensorState(State state);
        void isEnabled(bool enabled);
        bool isSensorReady();
        virtual list<code> getCodeList() {return list<code>();}
        virtual void writeToFile(ofstream &out);
        virtual string getSensorInfo();
        /*virtual bool isSensorReady();*/
};