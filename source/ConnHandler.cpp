#include "ConnHandler.h"

ConnHandler::ConnHandler() {
    setupServerSocket();
    startClientServerComunication();
}

void ConnHandler::setupServerSocket() {
    #ifndef RPI
        WSADATA WSAData;
        if (WSAStartup(MAKEWORD(2, 2), &WSAData)){
            throw "WSA Error";
        }
    #endif

    struct sockaddr_in *address;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    char recvbuf[512];
    int recvbuflen = 512;

    int iResult = getaddrinfo(LAN_IP, PORT, &hints, &result);
    if (iResult != 0) {
        throw "Getaddrinfo failed with error: " +  iResult;
    }

    //TEST
    address = (struct sockaddr_in *)(result->ai_addr);
    char *ciao = inet_ntoa(address->sin_addr);
    std::cout << address->sin_port << std::endl;
    //

    serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (bind(serverSocket, result->ai_addr, (int)result->ai_addrlen)) {
        throw "Bind failed with error: " + WSAGetLastError();
    }
    
    freeaddrinfo(result);

    iResult = listen(serverSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        throw "Listen failed with error:" + WSAGetLastError();
    }
}

void ConnHandler::startClientServerComunication() {
    const int MAX_CLIENTS = 10;
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
    #ifndef RPI
        WSACleanup();
    #endif
}
void ConnHandler::clientThread(int clientSocket) {
    #ifdef WIN32
        tcp_keepalive KeepAlive;
        DWORD dJunk;
        KeepAlive.onoff = 1;
        KeepAlive.keepalivetime = 1000;
        KeepAlive.keepaliveinterval = 100;

        WSAIoctl(clientSocket, SIO_KEEPALIVE_VALS, &KeepAlive, sizeof( KeepAlive ), NULL, 0, &dJunk, NULL, NULL );
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
            
        } 
    }while(message != core.fail);
    cout<<"OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO"<<endl;
    closesocket(clientSocket);
}