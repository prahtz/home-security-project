#include "State.h"
#include <list>
#include <fstream>
#include <iostream>

using namespace std;

class Sensor {
    private:
        int sensorID;
        State sensorState;
    public:
        Sensor() {};
        Sensor(int sensorID, State sensorState);
        int getSensorID();
        State getSensorState();
        void setSensorID(int sensorID);
        void setSensorState(State state);
        bool isSensorReady();
        virtual list<int> getCodeList() {return list<int>();}
        virtual void writeToFile(ofstream &out);
        /*virtual bool isSensorReady();*/
};