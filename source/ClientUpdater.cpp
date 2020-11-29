#include "ClientUpdater.h"

list<future<void>> ClientUpdater::clientThreads;
list<TCPComm*> ClientUpdater::tcpCommList;

void ClientUpdater::sendUpdatesToClients() {
    list<future<void>>::iterator it = clientThreads.begin();
    list<TCPComm*>::iterator itTCP = tcpCommList.begin();
    string message = EventHandler::alarmActivated ? message::ALARM_ACTIVE : message::ALARM_INACTIVE;
    while(it != clientThreads.end()) {
        if(!is_ready((*it)))
            (*itTCP)->sendMessage(message);
        ++it;
        ++itTCP;
    }
}