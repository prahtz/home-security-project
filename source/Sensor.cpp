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

void Sensor::setSensorID(int sensorID) {
    this->sensorID = sensorID;
}
void Sensor::setSensorState(State state) {
    this->sensorState = state;
}

bool Sensor::isSensorReady() {
    return sensorState == State::CLOSED;
}

void Sensor::writeToFile(ofstream &out) {
    out << sensorID << ";" << sensorState << ";";
}

string Sensor::getSensorInfo() {
    return to_string(sensorID) + SEPARATOR + to_string(sensorState);
}

