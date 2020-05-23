#include "State.h"

class Sensor {
    private:
        int sensorID;
        State sensorState;
    public:
        Sensor(){};
        Sensor(int sensorID, State sensorState);
        int getSensorID();
        State getSensorState();
        virtual bool isSensorReady();
};