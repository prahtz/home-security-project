#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <list>
#include <algorithm>
#include <thread>
#include <future>
#include <stdlib.h>
#include <sstream>
#include <map>
#include <memory>
#include "EventHandler.h"
#include "SensorTypes.h"
#include "Action.h"
#include "Exceptions.h"
#include "TCPComm.h"
#include "ClientUpdater.h"
#include "Logger.h"
#include "CriticalSection.hpp"

using namespace std;

class Core{
    private:
        atomic<bool> abortProcedure;
        Receiver receiver;
        Transmitter transmitter;
        FirebaseMessagesHandler firebaseMessagesHandler;
        EventHandler eventHandler;
        thread receiverThread, eventHandlerThread, transmitterThread, firebaseMessagesHandlerThread;
        list<string> messageBuffer;
        mutex mCore;
        
        void waitOnCondition(atomic<bool> &abort, atomic<bool> &cond);
        void registerCloseCode(TCPComm& tcpComm, DoorSensor *ds, atomic<bool> &abort);
        void registerOpenCode(TCPComm& tcpComm, DoorSensor *ds, atomic<bool> &abort);
        void registerSensorName(TCPComm& tcpComm, DoorSensor *ds, atomic<bool> &abort, string &sensorName);
        bool pinCheck(TCPComm& tcpComm);

    public:
        Core();
        mutex& getMutex() {return mCore;};
        void startService();
        void activateAlarm(TCPComm& tcpComm);
        void deactivateAlarm(TCPComm& tcpComm);
        void sensorList(TCPComm& tcpComm);
        void updateBattery(TCPComm& tcpComm);
        void removeSensor(TCPComm& tcpComm);
        void deactivateSensor(TCPComm& tcpComm);
        void activateSensor(TCPComm& tcpComm);
        bool isAlarmReady();
        void registerNewDoorSensor(TCPComm& tcpComm);
        void handleFirebaseToken(TCPComm& tcpComm);
        void setupNewPIN(TCPComm& tcpComm);
        void setupFirstPIN(TCPComm &tcpComm);
        
};