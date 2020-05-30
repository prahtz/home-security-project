#include "Receiver.h"
#include "DoorSensor.h"
#include "Action.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>

class EventHandler {
    private:
        Receiver* receiver;
        list<Sensor*>* knownSensorList;
        map<code, pair<Action, Sensor*>*>* codeMap;

        Sensor* getSensorByCode(code codeReceived, bool* knownCode);
        void onSensorOpen(Sensor* sensor);
        void onSensorClose(Sensor* sensor);
        
    public:
        std::condition_variable codeAvailable, newCodeAvailable;
        std::mutex mSensorList, mNewCode;
        atomic<bool> registerCode, codeArrived;
        atomic<code> newCode;

        EventHandler(){}
        EventHandler(Receiver* receiver, list<Sensor*>* knownSensorList, map<code, pair<Action, Sensor*>*>* codeMap);

        void startListening(list<Sensor*> &knownSensorList);


};