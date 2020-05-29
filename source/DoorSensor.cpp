#include "DoorSensor.h"

DoorSensor::DoorSensor() : Sensor() {
    
}

DoorSensor::DoorSensor(int sensorID, State state, int openCode, int closeCode) : Sensor(sensorID, state){
    this->openCode = openCode;
    this->closeCode = closeCode;
}

list<int> DoorSensor::getCodeList() {
    list<int> codeList;
    codeList.push_front(closeCode);
    codeList.push_front(openCode);
    return codeList;
}

void DoorSensor::writeToFile(ofstream &out) {
    out << "0;";
    Sensor::writeToFile(out);
    out<<openCode<<";"<<closeCode<<endl;
}

void DoorSensor::setOpenCode(int openCode) {
    this->openCode = openCode;
}

void DoorSensor::setCloseCode(int closeCode) {
    this->closeCode = closeCode;
}