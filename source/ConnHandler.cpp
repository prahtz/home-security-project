#include "ConnHandler.h"

ConnHandler::ConnHandler() {
    setupServerSocket();
    startClientServerComunication();
}

void ConnHandler::setupServerSocket() {
    #ifdef _WIN32
        WSADATA WSAData;
        if (WSAStartup(MAKEWORD(2, 2), &WSAData)){
            throw "WSA Error";
        }
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = PROTOCOL;
    hints.ai_flags = AI_PASSIVE;
    char recvbuf[BUFSIZ];
    int recvbuflen = BUFSIZ;

    int iResult = getaddrinfo(LAN_IP, PORT, &hints, &result);
    if (iResult != 0) {
        throw "Getaddrinfo failed with error: " +  iResult;
    }
    
    serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (bind(serverSocket, result->ai_addr, (int)result->ai_addrlen)) {
        throw "Bind failed with error: " + WSAGetLastError();
    }
    
    freeaddrinfo(result);

    iResult = listen(serverSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        throw "Listen failed with error:" + WSAGetLastError();
    }
    #else

    struct sockaddr_in serv_addr, cli_addr;
    serverSocket = socket(DOMAIN, TRANSPORT, 0);
    cout << serverSocket <<endl;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    int portno = atoi(PORT);
    struct linger l;
    l.l_onoff = 1;
    l.l_linger = 0;
    int reuseaddr = 1;
    int reuseport = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
    //setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int));
    //setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &reuseaddr, sizeof(int));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(LAN_IP);
    serv_addr.sin_port = htons(portno);

    if (bind(serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "qua" << endl;
        throw "Bind failed";
    }
    listen(serverSocket, MAX_CLIENTS);
    #endif
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
        cout<<i<<endl;
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
    #ifdef _WIN32
        WSACleanup();
    #endif
}
void ConnHandler::clientThread(int clientSocket) {
    #ifdef _WIN32
        tcp_keepalive KeepAlive;
        DWORD dJunk;
        KeepAlive.onoff = 1;
        KeepAlive.keepalivetime = 1000;
        KeepAlive.keepaliveinterval = 1000;

        WSAIoctl(clientSocket, SIO_KEEPALIVE_VALS, &KeepAlive, sizeof( KeepAlive ), NULL, 0, &dJunk, NULL, NULL );
    #else
        int yes = 1;
        setsockopt(clientSocket, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));
        int idle = 1;
        setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));

        int interval = 1;
        setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

        int maxpkt = 10;
        setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));

    #endif

    char *buf = new char[BUFSIZ];
    string message;
    do {
        message = core.getMessage(clientSocket);
        if(message == Message::ACTIVATE_ALARM) {
            mCore.lock();
            core.activateAlarm(clientSocket);
            mCore.unlock();
        }else if (message == Message::DEACTIVATE_ALARM){
            mCore.lock();
            core.deactivateAlarm(clientSocket);
            mCore.unlock();
        }else if (message == Message::REGISTER_DOORSENSOR){
            mCore.lock();
            core.registerNewDoorSensor(clientSocket);
            mCore.unlock();
        }else if (message == Message::SENSOR_LIST){
            mCore.lock();
            core.sensorList(clientSocket);
            mCore.unlock();
        } 
        else if(message.substr(message.length() - Message::DEACTIVATE_SENSOR.length(), message.length()) == Message::DEACTIVATE_SENSOR) {
            mCore.lock();
            core.deactivateSensor(clientSocket, message);
            mCore.unlock();
        }
        else if(message.substr(message.length() - Message::ACTIVATE_SENSOR.length(), message.length()) == Message::ACTIVATE_SENSOR) {
            mCore.lock();
            core.activateSensor(clientSocket, message);
            mCore.unlock();
        }
    }while(message != core.fail);
    cout<<"OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO"<<endl;
    closesocket(clientSocket);
}