#include "ClientUpdater.h"

ClientUpdater::ClientUpdater() {

}

void ClientUpdater::sendUpdatesToClients() {
    list<string> messages = {critical_section::alarmActivated ? message::ALARM_ACTIVE : message::ALARM_INACTIVE, 
                            critical_section::defensesActivated ? message::DEFENSES_ACTIVE : message::DEFENSES_INACTIVE};
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