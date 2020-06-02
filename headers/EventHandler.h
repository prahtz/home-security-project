#include "Receiver.h"
#include "DoorSensor.h"
#include "Action.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>

#define KNOWN_PATH "./csv/known.csv"

class EventHandler {
    private:
        Receiver* receiver;
        list<Sensor*>* knownSensorList;
        map<code, pair<Action, Sensor*>*>* codeMap;

        void updateKnownFile();
        void onSensorOpen(Sensor* sensor);
        void onSensorClose(Sensor* sensor);
        
    public:
        std::condition_variable codeAvailable, newCodeAvailable;
        std::mutex mSensorList, mNewCode, mFile;
        atomic<bool> registerCode, codeArrived;
        atomic<code> newCode;

        EventHandler(){}
        EventHandler(Receiver* receiver, list<Sensor*>* knownSensorList, map<code, pair<Action, Sensor*>*>* codeMap);

        void startListening();


};