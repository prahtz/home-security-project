#include "TCPComm.h"

condition_variable TCPComm::sharedCondition;
mutex TCPComm::mSharedCondition;

void TCPComm::fillBuffer(char *buffer, string s)
{
    for (int i = 0; i < s.length(); i++)
        buffer[i] = s[i];
}


TCPComm::TCPComm(int clientSocket) {
    this->clientSocket = clientSocket;
    messageAvailable = false;
    
}

void TCPComm::startReceive() {
    string message = "";
    char *buf = new char[BUFSIZ];
    bool go = true;
    while(go) {
        int r = recv(clientSocket, buf, sizeof(buf), 0);
        if (r == 0 || r == SOCKET_ERROR)
        {
            mMessageBuffer.lock();
            messageBuffer.push_front(message::FAIL);
            mMessageBuffer.unlock();
            delete buf;
            messageAvailable = true;
            go = false;
            statical::sharedCondition.notify_all();
        }
        else {
            message = message + ((string)buf).substr(0, r);
            unsigned int pos = message.find(message::EOM);
            while (pos != string::npos)
            {
                mMessageBuffer.lock();
                messageBuffer.push_front(message.substr(0, message.length() - message::EOM.length()));
                messageAvailable = true;
                mMessageBuffer.unlock();
                statical::sharedCondition.notify_all();
               
                if(message.length() != pos + message::EOM.length()) 
                    message = message.substr(pos + message::EOM.length(), message.length());
                else
                    message = "";
                pos = message.find(message::EOM);
            }   
        }
    }
}

bool TCPComm::isAvailable() {
    return messageAvailable;
}

void TCPComm::sendMessage(string message)
{
    char *buf = new char[BUFSIZ];
    fillBuffer(buf, message + message::EOM);
    send(clientSocket, buf, message.length() + message::EOM.length(), 0);
}

string TCPComm::getMessage() {
    mMessageBuffer.lock();
    if(!messageAvailable) { return "";}
    string message = messageBuffer.back();
    messageBuffer.pop_back();
    messageAvailable = !messageBuffer.empty();
    mMessageBuffer.unlock();
    return message;
}

void TCPComm::flush() {
    mMessageBuffer.lock();
    messageBuffer.erase(messageBuffer.begin(), messageBuffer.end());
    messageAvailable = !messageBuffer.empty();
    cout<< "FLUSH: " << messageBuffer.empty() << endl;
    mMessageBuffer.unlock();
    
}