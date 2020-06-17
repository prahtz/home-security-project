#if defined(WIN32)
    #include <winsock2.h>
    #define LAN_IP "192.168.1.50"
    #include <ws2tcpip.h>
    #include <Iphlpapi.h>
#else
    #include <sys/types.h> 
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <string.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define LAN_IP "192.168.1.48"


   
#endif

#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <thread>
#include <future>
#include <exception>
#include <stdlib.h>
#include <sstream>
#include <map>
#include "AlarmType.h"
#include "EventHandler.h"
#include "SensorTypes.h"
#include "Action.h"
#include "Message.h"
#include "MessageType.h"


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
        void setupKnownSensors();
        void startClientServerComunication();
        int getNewSensorID();
        bool isFutureReady(shared_future<string> fut);
        

    public:
        Core(); 
        string eom = "//eom";
        string fail = "//null";
        bool addSensorToList(Sensor* s, list<Sensor*>* sensorList);
        bool removeSensorFromList(Sensor* s, list<Sensor*>* sensorList);
        void activateAlarm(AlarmType at);
        bool isAlarmReady(AlarmType at);
        void registerNewDoorSensor(int clientSocke);
        void writeSensorToFile(Sensor* s);
        string getMessage(int clientSocket);
        void sendMessage(int clientSocket, string message);
        void fillBuffer(char* buffer, string s);
};