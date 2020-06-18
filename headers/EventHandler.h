#include "Receiver.h"
#include "Transmitter.h"
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

        Transmitter transmitter;
        code syrenCode;

        void updateKnownFile();
        void onSensorOpen(Sensor* sensor);
        void onSensorClose(Sensor* sensor);
        void activateDefenses();
        
    public:
        std::condition_variable codeAvailable, newCodeAvailable, alarmDeactivated;
        std::mutex mSensorList, mNewCode, mFile, mAlarm;
        atomic<bool> registerCode, codeArrived, alarmActivated;
        atomic<code> newCode;

        EventHandler(){}
        EventHandler(Receiver* receiver, list<Sensor*>* knownSensorList, map<code, pair<Action, Sensor*>*>* codeMap);

        void startListening();


};