#include "Sensor.h"

Sensor::Sensor(int sensorID, State sensorState, bool enabled) {
    this->sensorID = sensorID;
    this->sensorState = sensorState;
    this->enabled = enabled;
}

int Sensor::getSensorID() {
    return sensorID;
}
State Sensor::getSensorState() {
    return sensorState;
}

void Sensor::setSensorID(int sensorID) {
    this->sensorID = sensorID;
}
void Sensor::setSensorState(State state) {
    this->sensorState = state;
}

void Sensor::isEnabled(bool enabled) {
    this->enabled = enabled;
}

bool Sensor::isEnabled() {
    return enabled;
}

bool Sensor::isSensorReady() {
    return sensorState == State::CLOSED || !enabled;
}

void Sensor::writeToFile(ofstream &out) {
    out << sensorID << ";" << sensorState << ";" << enabled << ";";
}

string Sensor::getSensorInfo() {
    return to_string(sensorID) + SEPARATOR + to_string(sensorState) + SEPARATOR + to_string(enabled);
}

