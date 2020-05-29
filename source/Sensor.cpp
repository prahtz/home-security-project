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

//TEST
bool Sensor::isSensorReady() {
    if(sensorState == State::CLOSED)
        return true;
    return false;
}

void Sensor::writeToFile(ofstream &out) {
    out << sensorID << ";" << sensorState << ";";
}
/*
list<int> Sensor::getCodeList() {
    cout<<"CAZZO";
    return list<int>();
}*/
