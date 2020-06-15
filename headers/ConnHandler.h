#include "Core.h"

using namespace std;

#define PORT "33470"
#define DOMAIN  AF_INET
#define TRANSPORT SOCK_STREAM
#define PROTOCOL IPPROTO_TCP
#define DNS_DOMAIN "prah.homepc.it"
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

class ConnHandler {
    private:
        int serverSocket;
        Core core;

        mutex mCore;

        void setupServerSocket();
        void startClientServerComunication();
    public:
        ConnHandler();
        void clientThread(int clientSocket);

};