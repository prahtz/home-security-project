
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
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <iostream>

#include "Definitions.h"
#include "Message.h"

using namespace std;

class TCPComm {
    private:
        int clientSocket;
        list<string> messageBuffer;
        
        atomic<bool> messageAvailable;
        mutex mMessageBuffer;

        void fillBuffer(char *buffer, string s);
    public:
        static condition_variable sharedCondition;
        static mutex mSharedCondition;
        TCPComm(){};
        TCPComm(int clientSocket);
        void startReceive();
        void sendMessage(string message);
        string getMessage();
        bool isAvailable();
        void flush();
};
