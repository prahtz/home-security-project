#include "ConnHandler.h"

ConnHandler::ConnHandler() {
    setupServerSocket();
    startClientServerComunication();
}

ConnHandler::ConnHandler(string lan_IP, string port) {
    this->lan_IP = lan_IP;
    this->port = port;
    setupServerSocket();
    startClientServerComunication();
}

void ConnHandler::setupServerSocket() {
    struct sockaddr_in serv_addr, cli_addr;
    serverSocket = socket(DOMAIN, TRANSPORT, 0);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    int portno = atoi(port.c_str());
    struct linger l;
    l.l_onoff = 1;
    l.l_linger = 0;
    int reuseaddr = 1;
    int reuseport = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
    //setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int));
    //setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &reuseaddr, sizeof(int));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(lan_IP.c_str());
    serv_addr.sin_port = htons(portno);

    if (bind(serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        throw "Bind failed";
    }
    listen(serverSocket, MAX_CLIENTS);
}

void ConnHandler::startClientServerComunication() {
    
    list<std::thread> clientThreads;
    int i = 0;
    while(true) {
        int clientSocket = INVALID_SOCKET;
        while(clientSocket == INVALID_SOCKET) 
            clientSocket = accept(serverSocket, NULL, NULL);
        clientThreads.push_back(std::thread(&ConnHandler::clientThread, this, clientSocket));
        i++;
        if(i >= MAX_CLIENTS) {
            list<thread>::iterator it = clientThreads.begin();
            while(i >= MAX_CLIENTS) {
                for(int j = 0; j < clientThreads.size(); j++) {
                    if((*it).joinable()) {
                        (*it).join();
                        it = clientThreads.erase(it);
                        i--;
                    }
                    else
                        ++it;
                }
                usleep(1000);
            }
                
        }
    }
    closesocket(serverSocket);
}
void ConnHandler::clientThread(int clientSocket) {
    int yes = 1;
    setsockopt(clientSocket, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));
    int idle = 1;
    setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));

    int interval = 1;
    setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

    int maxpkt = 10;
    setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));

    char *buf = new char[BUFSIZ];
    string message;
    TCPComm tcpComm(clientSocket);
    thread tcpThread = thread(&TCPComm::startReceive, &tcpComm);
    do {
        unique_lock<mutex> clientLock(statical::mSharedCondition);
        statical::sharedCondition.wait(clientLock, [this, &tcpComm] {
            return tcpComm.isAvailable();
        });
        clientLock.unlock();
        message = tcpComm.getMessage();
        if(message == message::ACTIVATE_ALARM) {
            mCore.lock();
            core.activateAlarm(tcpComm);
            mCore.unlock();
        }else if (message == message::DEACTIVATE_ALARM){
            mCore.lock();
            core.deactivateAlarm(tcpComm);
            mCore.unlock();
        }else if (message == message::REGISTER_DOORSENSOR){
            mCore.lock();
            core.registerNewDoorSensor(tcpComm);
            mCore.unlock();
        }else if (message == message::SENSOR_LIST){
            mCore.lock();
            core.sensorList(tcpComm);
            mCore.unlock();
        }
        else if(message.length() >= message::DEACTIVATE_SENSOR.length() && message.substr(message.length() - message::DEACTIVATE_SENSOR.length(), message.length()) == message::DEACTIVATE_SENSOR) {
            mCore.lock();
            core.deactivateSensor(tcpComm, message);
            mCore.unlock();
        }
        else if(message.length() >= message::ACTIVATE_SENSOR.length() && message.substr(message.length() - message::ACTIVATE_SENSOR.length(), message.length()) == message::ACTIVATE_SENSOR) {
            mCore.lock();
            core.activateSensor(tcpComm, message);
            mCore.unlock();
        }
        else if(message.length() >= message::REMOVE_SENSOR.length() && message.substr(message.length() - message::REMOVE_SENSOR.length(), message.length()) == message::REMOVE_SENSOR) {
            mCore.lock();
            core.removeSensor(tcpComm, message);
            mCore.unlock();
        }
        else if(message != message::FAIL) {
            mCore.lock();
            core.handleFirebaseToken(message);
            mCore.unlock();
        }
    }while(message != message::FAIL);
    cout<<"Closing socket..."<<endl;
    tcpThread.join();
    closesocket(clientSocket);
}