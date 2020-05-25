#include "Sensor.h"

Sensor::Sensor(int sensorID, State sensorState) {
    this->sensorID = sensorID;
    this->sensorState = sensorState;
}

int Sensor::getSensorID() {
    return sensorID;
}

State Sensor::getSensorState() {
    return sensorState;
}

//TEST
bool Sensor::isSensorReady() {
    if(sensorState == CLOSED)
        return true;
    return false;
}