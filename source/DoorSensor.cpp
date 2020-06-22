#include "DoorSensor.h"

DoorSensor::DoorSensor() : Sensor() {
    
}

DoorSensor::DoorSensor(int sensorID, State state, bool enabled, code openCode, code closeCode, string sensorName) : Sensor(sensorID, state, enabled){
    this->openCode = openCode;
    this->closeCode = closeCode;
    this->sensorName = sensorName;
}

list<code> DoorSensor::getCodeList() {
    list<code> codeList;
    codeList.push_front(closeCode);
    codeList.push_front(openCode);
    return codeList;
}

void DoorSensor::setOpenCode(code openCode) {
    this->openCode = openCode;
}

void DoorSensor::setCloseCode(code closeCode) {
    this->closeCode = closeCode;
}

void DoorSensor::setSensorName(string sensorName) {
    this->sensorName = sensorName;
}

code DoorSensor::getOpenCode() {
    return openCode;
}
code DoorSensor::getCloseCode() {
    return closeCode;
}

string DoorSensor::getSensorName() {
    return sensorName;
}

void DoorSensor::writeToFile(ofstream &out) {
    out << DOOR_SENSOR << ";";
    Sensor::writeToFile(out);
    out<<openCode<<";"<<closeCode<<";"<<sensorName<<endl;
}

string DoorSensor::getSensorInfo() {
    return to_string(DOOR_SENSOR) + SEPARATOR + Sensor::getSensorInfo() + SEPARATOR + to_string(openCode) + SEPARATOR + to_string(closeCode) + SEPARATOR + sensorName;
}