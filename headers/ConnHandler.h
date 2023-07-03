#pragma once

#include "Core.h"
#include "ClientUpdater.h"
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

using namespace std;

#define DOMAIN  AF_INET
#define TRANSPORT SOCK_STREAM
#define PROTOCOL IPPROTO_TCP

class ConnHandler {
    private:
        int serverSocket;
        Core core;
        const int MAX_CLIENTS = 10;
        mutex mCore;
        string lan_IP;
        string port;

        void setupServerSocket();
        void initClientSocket(int clientSocket);
        void startClientServerComunication();

        int closesocket(int socket) {
            return close(socket);
        }   
    public:
        ConnHandler();
        ConnHandler(string lan_IP, string port);
        void clientThread(int clientSocket, TCPComm& tcpComm); 
};