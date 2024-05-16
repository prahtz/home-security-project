#include "DoorSensor.h"

DoorSensor::DoorSensor() : Sensor() {
    
}

DoorSensor::DoorSensor(int sensorID, State state, bool enabled, code openCode, code closeCode, string sensorName) : Sensor(sensorID, state, enabled){
    this->openCode = openCode;
    this->closeCode = closeCode;
    this->sensorName = sensorName;
}

list<pair<code, Action>> DoorSensor::getCodeActionList() {
    list<pair<code, Action>> codeActionList;
    codeActionList.push_front({closeCode, CLOSE});
    codeActionList.push_front({openCode, OPEN});
    codeActionList.push_front({getBatteryLowCode(), BATTERY_LOW});
    return codeActionList;
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

code DoorSensor::getBatteryLowCode() {
    return min(openCode, closeCode) - BATTERY_LOW_SHIFT;
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