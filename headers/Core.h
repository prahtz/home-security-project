#ifdef _WIN32
    #include <winsock2.h>
    #define LAN_IP "192.168.1.50"
    #include <ws2tcpip.h>
#else
    #include <sys/types.h> 
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
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
#include <stdlib.h>
#include <sstream>
#include <map>
#include "AlarmType.h"
#include "EventHandler.h"
#include "SensorTypes.h"
#include "Action.h"
#include "Message.h"
#include "MessageType.h"
#include "Exceptions.h"


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

        void setupKnownSensors();
        void startClientServerComunication();
        int getNewSensorID();
        bool isFutureReady(shared_future<string> fut);
        string getMessageAndNotify(int clientSocket);
        

    public:
        Core(); 
        string eom = "//eom";
        string fail = "//null";
        bool addSensorToList(Sensor* s);
        bool removeSensorFromList(Sensor* s);
        void activateAlarm(int clientSocket);
        void deactivateAlarm(int clientSocket);
        void sensorList(int clientSocket);
        void removeSensor(int clientSocket, string message);
        void deactivateSensor(int clientSocket, string message);
        void activateSensor(int clientSocket, string message);
        bool isAlarmReady();
        void registerNewDoorSensor(int clientSocke);
        void writeSensorToFile(Sensor* s);
        string getMessage(int clientSocket);
        void sendMessage(int clientSocket, string message);
        void fillBuffer(char* buffer, string s);
};