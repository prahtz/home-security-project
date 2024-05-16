#pragma once
#include "Sensor.h"
#include <fstream>

class DoorSensor : public Sensor {
    private:
        code openCode;
        code closeCode;
        string sensorName;
    public:
        DoorSensor();
        DoorSensor(int sensorID, State state, bool enabled, code openCode, code closeCode, string sensorName);
        list<pair<code, Action>> getCodeActionList() override;
        void setOpenCode(code openCode);
        void setCloseCode(code closeCode);
        void setSensorName(string sensorName);
        code getOpenCode();
        code getCloseCode();
        code getBatteryLowCode();
        string getSensorName();
        virtual void writeToFile(ofstream &out);
        virtual string getSensorInfo();   
};