#include "ConnHandler.h"

ConnHandler::ConnHandler() {
    setupServerSocket();
    startClientServerComunication();
}

ConnHandler::ConnHandler(string lan_IP, string port) {
    this->lan_IP = lan_IP;
    this->port = port;
    try{
    setupServerSocket();
    }
    catch(exception e) {
        std::cout << e.what() << std::endl;
    }
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
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(lan_IP.c_str());
    serv_addr.sin_port = htons(portno);

    if (bind(serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        throw "Bind failed";
    }
    listen(serverSocket, MAX_CLIENTS);
}

void ConnHandler::startClientServerComunication() {
    int i = 0;
    while(true) {
        int clientSocket = INVALID_SOCKET;
        while(clientSocket == INVALID_SOCKET) 
            clientSocket = accept(serverSocket, NULL, NULL);
        initClientSocket(clientSocket);
        TCPComm* tcpComm = new TCPComm(clientSocket);
        ClientUpdater::clientThreads.push_back(std::async(std::launch::async, &ConnHandler::clientThread, this, clientSocket, tcpComm));
        ClientUpdater::tcpCommList.push_back(tcpComm);
        i++;
        if(i > MAX_CLIENTS) {
            list<future<void>>::iterator it = ClientUpdater::clientThreads.begin();
            list<TCPComm*>::iterator itTCP = ClientUpdater::tcpCommList.begin();
            while(i >= MAX_CLIENTS) {
                while(it != ClientUpdater::clientThreads.end()) {
                    if(ClientUpdater::is_ready((*it))) {
                        it = ClientUpdater::clientThreads.erase(it);
                        itTCP = ClientUpdater::tcpCommList.erase(itTCP);
                        i--;
                    }
                    else {
                        ++it;
                        ++itTCP;
                    }
                }
            }
                
        }
    }
    closesocket(serverSocket);
}

void ConnHandler::initClientSocket(int clientSocket) {
    int yes = 1;
    setsockopt(clientSocket, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));
    int idle = 1;
    setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));

    int interval = 1;
    setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

    int maxpkt = 10;
    setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));
}

void ConnHandler::clientThread(int clientSocket, TCPComm* tcpComm) {
    string message;
    thread tcpThread = thread(&TCPComm::startReceive, tcpComm);
    do {
        unique_lock<mutex> clientLock(statical::mSharedCondition);
        statical::sharedCondition.wait(clientLock, [this, tcpComm] {
            return tcpComm->isAvailable();
        });
        clientLock.unlock();
        message = tcpComm->getMessage();
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
        else if(message == message::INFO_REQUEST) {
            mCore.lock();
            ClientUpdater::sendUpdatesToClients();
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
        else if(message.length() >= message::UPDATE_BATTERY.length() && message.substr(message.length() - message::UPDATE_BATTERY.length(), message.length()) == message::UPDATE_BATTERY) {
            mCore.lock();
            core.updateBattery(tcpComm, message);
            mCore.unlock();
        }
        else if(message != message::FAIL) {
            mCore.lock();
            core.handleFirebaseToken(message);
            mCore.unlock();
        }
    }while(message != message::FAIL);
    closesocket(clientSocket);
    tcpThread.join();
    cout<<"Closing socket..."<<endl;
}