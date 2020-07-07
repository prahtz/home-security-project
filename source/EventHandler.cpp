#include "EventHandler.h"

EventHandler::EventHandler(Receiver *receiver, Transmitter *transmitter, list<Sensor *> *knownSensorList, map<code, pair<Action, Sensor *> *> *codeMap)
{
    this->receiver = receiver;
    this->transmitter = transmitter;
    this->knownSensorList = knownSensorList;
    this->codeMap = codeMap;

    registerCode = false;
    codeArrived = false;
    alarmActivated = false;
    defensesActivated = false;
}


//TO TEST
void EventHandler::startListening()
{
    while (true)
    {

        unique_lock<mutex> receiverLock(receiver->mBuff);
        receiver->codeAvailable.wait(receiverLock, [this] { return !receiver->isBufferEmpty(); });

        code codeReceived = receiver->popCodeFromBuffer();
        map<code, pair<Action, Sensor *> *>::iterator mapIterator = codeMap->find(codeReceived);
        bool knownCode = codeMap->end() != mapIterator;
        if (knownCode)
        {
            Action action = mapIterator->second->first;
            Sensor *sensor = mapIterator->second->second;
            switch (action)
            {
            case OPEN:
                onSensorOpen(sensor);
                break;
            case CLOSE:
                onSensorClose(sensor);
            }
        }
        else if(codeReceived == ackActivateCode) {
            transmitter->mTransmit.lock();
            if(transmitter->getTransmittingCode() == activateSirenCode) {
                transmitter->isAckReceived(true);
                transmitter->mTransmit.unlock();
                transmitter->startTransmitting.notify_all();
            }
            else
                transmitter->mTransmit.unlock();
        }
        else if(codeReceived == ackDeactivateCode) {
            transmitter->mTransmit.lock();
            if(transmitter->getTransmittingCode() == deactivateSirenCode) {
                transmitter->isAckReceived(true);
                transmitter->mTransmit.unlock();
                transmitter->startTransmitting.notify_all();
            }
            else
                transmitter->mTransmit.unlock();
        }
        else if(codeReceived == tamperActiveCode) {
            transmitter->mTransmit.lock();
            defensesActivated = true;
            transmitter->addTransmittingCode(ackControlUnitCode, SEND_ACK);
            transmitter->mTransmit.unlock();
            transmitter->startTransmitting.notify_all();
        }
        else if(!isATransmittingCode(codeReceived)) 
        {
            //cout<<"NO " << codeReceived<<endl;
            cout << "EventHandler - codeReceived: " << codeReceived << endl;
            cout << "EventHandler - registerCode?: " << registerCode << endl;
            if (registerCode)
            {
                newCode = codeReceived;
                cout << "EventHandler - newCode: " << newCode << endl;
                codeArrived = true;
                newCodeAvailable.notify_all();
            }
        }
    }
}

//TO TEST
void EventHandler::onSensorOpen(Sensor *sensor)
{
    mSensorList.lock();
    if (alarmActivated && sensor->isEnabled())
        activateDefenses();
    sensor->setSensorState(OPENED);
    updateKnownFile();
    mSensorList.unlock();
}

//TO TEST
void EventHandler::onSensorClose(Sensor *sensor)
{
    mSensorList.lock();
    sensor->setSensorState(CLOSED);
    updateKnownFile();
    mSensorList.unlock();
}

bool EventHandler::isATransmittingCode(code codeReceived) {
    return codeReceived == activateSirenCode || codeReceived == deactivateSirenCode || codeReceived == ackControlUnitCode;
}

void EventHandler::updateKnownFile()
{
    mFile.lock();
    ofstream out(KNOWN_PATH, ios::trunc);
    for (Sensor *s : (*knownSensorList))
        s->writeToFile(out);
    out.close();
    mFile.unlock();
}

void EventHandler::activateDefenses()
{
    defensesActivated = true;
    transmitter->mTransmit.lock();
    if(transmitter->getTransmittingCode() != activateSirenCode) {
        transmitter->addTransmittingCode(activateSirenCode, WAIT_FOR_ACK);
        transmitter->mTransmit.unlock();
        transmitter->startTransmitting.notify_all();
    }
    else
        transmitter->mTransmit.unlock();
}