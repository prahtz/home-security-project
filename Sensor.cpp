
#include "Sensor.h"

Sensor::Sensor(int sensorID) {
    this->sensorID = sensorID;
}

int Sensor::getSensorID() {
    return sensorID;
}
