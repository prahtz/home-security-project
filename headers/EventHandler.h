#pragma once
#include "Receiver.h"
#include "Transmitter.h"
#include "DoorSensor.h"
#include "Action.h"
#include "FirebaseMessagesHandler.h"
#include "FirebaseNotification.h"
#include "Logger.h"
#include "CriticalSection.hpp"
#include <map>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>

class EventHandler {
    protected:
        Receiver* receiver;
        Transmitter* transmitter;
        FirebaseMessagesHandler* firebaseMessagesHandler;
  
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
        std::mutex mNewCode, mFile, mAlarm;
        atomic<bool> registerCode, codeArrived, defensesActivated;
        atomic<code> newCode;

        EventHandler(){}
        EventHandler(Receiver* receiver, Transmitter* transmitter, FirebaseMessagesHandler* firebaseMessagesHandler);
        void startListening();
};