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


using namespace std;

class Core{
    private:
        bool alarmActivated;
        list<Sensor*> activeSensorList;
        list<Sensor*> knownSensorList;
        map<code, pair<Action, Sensor*>*> codeMap;
        Receiver receiver;
        EventHandler eventHandler;
        thread receiverThread;
        thread eventHandlerThread;
        void setupReceiver();
        void setupKnownSensors();
        void setupCodeMap();
        int getNewSensorID();
    public:
        Core(); 
        bool addSensorToList(Sensor* s, list<Sensor*> sensorList);
        bool removeSensorFromList(Sensor* s, list<Sensor*> sensorList);
        void activateAlarm(AlarmType at);
        bool isAlarmReady(AlarmType at);
        //int receiveNewCode();
        void registerNewSensor();
};