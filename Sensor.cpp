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
