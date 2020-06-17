#include "Sensor.h"
#include <fstream>

class DoorSensor : public Sensor {
    private:
        code openCode;
        code closeCode;
        string sensorName;
    public:
        DoorSensor();
        DoorSensor(int sensorID, State state, int openCode, int closeCode, string sensorName);
        list<code> getCodeList();
        void setOpenCode(code openCode);
        void setCloseCode(code closeCode);
        void setSensorName(string sensorName);
        code getOpenCode();
        code getCloseCode();
        void writeToFile(ofstream &out);      
};