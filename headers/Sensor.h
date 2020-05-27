#include "State.h"

class Sensor {
    private:
        int sensorID;
        State sensorState;
    public:
        inline Sensor(){};
        Sensor(int sensorID, State sensorState);
        int getSensorID();
        State getSensorState();
        bool isSensorReady();
        /*virtual bool isSensorReady();*/
};