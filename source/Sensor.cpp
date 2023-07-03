#include "Sensor.h"

Sensor::Sensor(int sensorID, State sensorState, bool enabled) {
    this->sensorID = sensorID;
    this->sensorState = sensorState;
    this->enabled = enabled;
    this->charged = true;
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

void Sensor::isCharged(bool charged) {
    this->charged = charged;
}

void Sensor::isEnabled(bool enabled) {
    this->enabled = enabled;
}

bool Sensor::isCharged() {
    return charged;
}

bool Sensor::isEnabled() {
    return enabled;
}

bool Sensor::isSensorReady() {
    return sensorState == State::CLOSED || !enabled;
}

void Sensor::writeToFile(ofstream &out) {
    out << sensorID << SEPARATOR << sensorState << SEPARATOR << enabled << SEPARATOR << charged << SEPARATOR;
}

string Sensor::getSensorInfo() {
    return to_string(sensorID) + SEPARATOR + to_string(sensorState) + SEPARATOR + to_string(enabled) + SEPARATOR + to_string(charged);
}

bool Sensor::operator==(Sensor s) {
    return getSensorID() == s.getSensorID();
}

