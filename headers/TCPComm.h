
#pragma once
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#include <string>
#include <list>
#include <iostream>
#include <thread>

#include "Definitions.h"
#include "Message.h"
#include "Stream.h"
#include "Subscription.h"

using namespace std;

class TCPComm {
    protected:
        int clientSocket;
        Stream<string> stream;
        void fillBuffer(char *buffer, string s);
    public:
        TCPComm(int clientSocket);
        TCPComm();
        ~TCPComm();
        virtual void startReceive();
        virtual Stream<string>& getMessageStream();
        virtual void sendMessage(string message);
};
