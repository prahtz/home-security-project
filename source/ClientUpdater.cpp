#include "ClientUpdater.h"

list<future<void>> ClientUpdater::clientThreads;
list<TCPComm> ClientUpdater::tcpCommList;

void ClientUpdater::sendUpdatesToClients() {
    list<string> messages = {EventHandler::alarmActivated ? message::ALARM_ACTIVE : message::ALARM_INACTIVE, 
                            EventHandler::defensesActivated ? message::DEFENSES_ACTIVE : message::DEFENSES_INACTIVE};
    list<future<void>>::iterator it = clientThreads.begin();
    list<TCPComm>::iterator itTCP = tcpCommList.begin();
    while(it != clientThreads.end()) {
        if(!is_ready((*it)))
            for (string m : messages) {
                (*itTCP).sendMessage(m);
            }
        ++it;
        ++itTCP;
    }
}