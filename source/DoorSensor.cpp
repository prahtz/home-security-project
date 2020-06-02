#include "DoorSensor.h"

DoorSensor::DoorSensor() : Sensor() {
    
}

DoorSensor::DoorSensor(int sensorID, State state, code openCode, code closeCode) : Sensor(sensorID, state){
    this->openCode = openCode;
    this->closeCode = closeCode;
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
    out<<openCode<<";"<<closeCode<<endl;
}

void DoorSensor::setOpenCode(code openCode) {
    this->openCode = openCode;
}

void DoorSensor::setCloseCode(code closeCode) {
    this->closeCode = closeCode;
}

code DoorSensor::getOpenCode() {
    return openCode;
}
code DoorSensor::getCloseCode() {
    return closeCode;
}