#include "Sensor.h"
#include <fstream>

class DoorSensor : public Sensor {
    private:
        int openCode;
        int closeCode;
    public:
        DoorSensor();
        DoorSensor(int sensorID, State state, int openCode, int closeCode);
        list<int> getCodeList();
        void setOpenCode(int openCode);
        void setCloseCode(int closeCode);
        void writeToFile(ofstream &out);      
};