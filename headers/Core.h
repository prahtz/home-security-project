#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <thread>
#include <future>
#include <stdlib.h>
#include <sstream>
#include <map>
#include "AlarmType.h"
#include "EventHandler.h"
#include "SensorTypes.h"
#include "Action.h"
#include "MessageType.h"
#include "Exceptions.h"
#include "TCPComm.h"


#define PIN 25
#define KNOWN_PATH "./csv/known.csv"


using namespace std;

class Core{
    private:
        atomic<bool> abortProcedure;
        list<Sensor*> knownSensorList;
        map<code, pair<Action, Sensor*>*> codeMap;
        Receiver receiver;
        Transmitter transmitter;
        EventHandler eventHandler;
        thread receiverThread, eventHandlerThread, transmitterThread;
        list<string> messageBuffer;

        void setupKnownSensors();
        void startClientServerComunication();
        int getNewSensorID();
        bool isFutureReady(shared_future<string> fut);
        

    public:
        Core(); 
        bool addSensorToList(Sensor* s);
        bool removeSensorFromList(Sensor* s);
        void activateAlarm(TCPComm &tcpComm);
        void deactivateAlarm(TCPComm &tcpComm);
        void sensorList(TCPComm &tcpComm);
        void removeSensor(TCPComm &tcpComm, string message);
        void deactivateSensor(TCPComm &tcpComm, string message);
        void activateSensor(TCPComm &tcpComm, string message);
        bool isAlarmReady();
        void registerNewDoorSensor(TCPComm &tcpComm);
        void writeSensorToFile(Sensor* s);
};