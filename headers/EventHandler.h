#include "Receiver.h"
#include "Transmitter.h"
#include "DoorSensor.h"
#include "Action.h"
#include "FirebaseMessagesHandler.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>

#define KNOWN_PATH "./csv/known.csv"
#define TOKEN_PATH "./csv/tokens.csv"

class EventHandler {
    private:
        Receiver* receiver;
        Transmitter* transmitter;
        FirebaseMessagesHandler* firebaseMessagesHandler;
        list<Sensor*>* knownSensorList;
        map<code, pair<Action, Sensor*>*>* codeMap;
  
        void onSensorOpen(Sensor* sensor);
        void onSensorClose(Sensor* sensor);
        void onSensorBatteryLow(Sensor* sensor);
        void onAckActivateCode();
        void onAckDeactivateCode();
        void onTamperActiveCode();
        void onUnknownCode(code codeReceived);
        bool isTransmittingCode(code codeReceived);
        void activateDefenses();
        
    public:
        std::condition_variable codeAvailable, newCodeAvailable;
        std::mutex mSensorList, mNewCode, mFile, mAlarm;
        atomic<bool> registerCode, codeArrived, alarmActivated, defensesActivated;
        atomic<code> newCode;

        EventHandler(){}
        EventHandler(Receiver* receiver, Transmitter* transmitter, FirebaseMessagesHandler* firebaseMessagesHandler, list<Sensor*>* knownSensorList, map<code, pair<Action, Sensor*>*>* codeMap);

        void updateKnownFile();
        void startListening();


};