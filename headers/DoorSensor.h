#include "Sensor.h"
#include <fstream>

class DoorSensor : public Sensor {
    private:
        code openCode;
        code closeCode;
        string sensorName;
    public:
        DoorSensor();
        DoorSensor(int sensorID, State state, code openCode, code closeCode, string sensorName);
        list<code> getCodeList();
        void setOpenCode(code openCode);
        void setCloseCode(code closeCode);
        void setSensorName(string sensorName);
        code getOpenCode();
        code getCloseCode();
        string getSensorName();
        virtual void writeToFile(ofstream &out);   
        virtual string getSensorInfo();   
};