#include "Receiver.h"
#include "DoorSensor.h"
#include <iostream>
#include <algorithm>

class EventHandler {
    private:
        Receiver* receiver;
        list<Sensor*>* knownSensorList;
        Sensor* getSensorByCode(int codeReceived, bool* knownCode);
        
    public:
        int a = 0;
        EventHandler(){}
        EventHandler(Receiver* receiver);
        void startListening(list<Sensor*> &knownSensorList);


};