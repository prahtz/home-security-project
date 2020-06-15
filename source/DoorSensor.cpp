#include "DoorSensor.h"

DoorSensor::DoorSensor() : Sensor() {
    
}

DoorSensor::DoorSensor(int sensorID, State state, code openCode, code closeCode, string sensorName) : Sensor(sensorID, state){
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

void DoorSensor::writeToFile(ofstream &out) {
    out << DOOR_SENSOR << ";";
    Sensor::writeToFile(out);
    out<<openCode<<";"<<closeCode<<";"<<sensorName<<endl;
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