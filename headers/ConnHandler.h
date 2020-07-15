#pragma comment(lib, "Ws2_32.lib")

#include "Core.h"
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

using namespace std;

#define DOMAIN  AF_INET
#define TRANSPORT SOCK_STREAM
#define PROTOCOL IPPROTO_TCP
#define DNS_DOMAIN "prah.homepc.it"
#ifdef _WIN32
    #define PORT "33471"
#else
    #define PORT "33470"
#endif

class ConnHandler {
    private:
        int serverSocket;
        Core core;
        const int MAX_CLIENTS = 10;
        mutex mCore;

        void setupServerSocket();
        void startClientServerComunication();

        #ifndef _WIN32
        int closesocket(int socket) {
                return close(socket);
        }   
        #endif
    public:
        ConnHandler();
        void clientThread(int clientSocket);

};