#include "Sensor.h"
#include <fstream>

class DoorSensor : public Sensor {
    private:
        code openCode;
        code closeCode;
    public:
        DoorSensor();
        DoorSensor(int sensorID, State state, int openCode, int closeCode);
        list<code> getCodeList();
        void setOpenCode(code openCode);
        void setCloseCode(code closeCode);
        code getOpenCode();
        code getCloseCode();
        void writeToFile(ofstream &out);      
};