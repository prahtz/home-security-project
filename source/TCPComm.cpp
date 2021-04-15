#include "TCPComm.h"

void TCPComm::fillBuffer(char *buffer, string s)
{
    for (int i = 0; i < s.length(); i++)
        buffer[i] = s[i];
}


TCPComm::TCPComm(int clientSocket) {
    this->clientSocket = clientSocket; 
}

void TCPComm::startReceive() {
    string message = "";
    char *buf = new char[BUFSIZ];
    bool go = true;
    while(go) {
        int r = recv(clientSocket, buf, sizeof(buf), 0);
        if (r == 0 || r == SOCKET_ERROR)
        {
            stream.add(message::FAIL);
            delete buf;
            go = false;
        }
        else {
            message = message + ((string)buf).substr(0, r);
            size_t pos = message.find(message::EOM);
            while (pos != string::npos)
            {
                stream.add(message.substr(0, message.length() - message::EOM.length()));
                if(message.length() != pos + message::EOM.length()) 
                    message = message.substr(pos + message::EOM.length(), message.length());
                else
                    message = "";
                pos = message.find(message::EOM);
            }   
        }
    }
}

Stream<string>* TCPComm::getMessageStream() {
    return &stream;
}

void TCPComm::sendMessage(string message)
{
    char *buf = new char[BUFSIZ];
    fillBuffer(buf, message + message::EOM);
    send(clientSocket, buf, message.length() + message::EOM.length(), 0);
}

TCPComm::~TCPComm() {
}