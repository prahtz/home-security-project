#pragma once
#include "Receiver.h"
#include "Transmitter.h"
#include "DoorSensor.h"
#include "Action.h"
#include "FirebaseMessagesHandler.h"
#include "FirebaseNotification.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>

class EventHandler {
    private:
        Receiver* receiver;
        Transmitter* transmitter;
        FirebaseMessagesHandler* firebaseMessagesHandler;
        list<Sensor*>* knownSensorList;
        list<string> *tokenList;
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
        static atomic<bool> alarmActivated;

        std::condition_variable codeAvailable, newCodeAvailable;
        std::mutex mSensorList, mNewCode, mFile, mAlarm;
        atomic<bool> registerCode, codeArrived, defensesActivated;
        atomic<code> newCode;

        EventHandler(){}
        EventHandler(Receiver* receiver, Transmitter* transmitter, FirebaseMessagesHandler* firebaseMessagesHandler, list<Sensor*>* knownSensorList, list<string> *tokenList, map<code, pair<Action, Sensor*>*>* codeMap);

        void updateKnownFile();
        void startListening();


};