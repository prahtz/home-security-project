#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <thread>
#include <exception>
#include <stdlib.h>
#include <sstream>
#include <map>
#include "AlarmType.h"
#include "EventHandler.h"
#include "SensorTypes.h"
#include "Action.h"


#define PIN 27
#define KNOWN_PATH "./csv/known.csv"


using namespace std;

class Core{
    private:
        bool alarmActivated;
        list<Sensor*> knownSensorList, activeSensorList;
        map<code, pair<Action, Sensor*>*> codeMap;
        Receiver receiver;
        EventHandler eventHandler;
        thread receiverThread, eventHandlerThread;
        void setupReceiver();
        void setupKnownSensors();
        int getNewSensorID();
    public:
        Core(); 
        bool addSensorToList(Sensor* s, list<Sensor*>* sensorList);
        bool removeSensorFromList(Sensor* s, list<Sensor*>* sensorList);
        void activateAlarm(AlarmType at);
        bool isAlarmReady(AlarmType at);
        void registerNewDoorSensor();
        void writeSensorToFile(Sensor* s);
};