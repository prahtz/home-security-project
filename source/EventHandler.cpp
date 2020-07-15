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
        bool sensorCode = codeMap->end() != mapIterator;
        if (sensorCode)
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
                    break;
                case BATTERY_LOW:
                    onSensorBatteryLow(sensor);
                
            }
        }
        else {
            switch(codeReceived)
            {
                case ackActivateCode:
                    onAckActivateCode();
                    break;
                case ackDeactivateCode:
                    onAckDeactivateCode();
                    break;
                case tamperActiveCode:
                    onTamperActiveCode();
                    break;
                default:
                    onUnknownCode(codeReceived);
                    
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

void EventHandler::onSensorBatteryLow(Sensor *sensor)
{
    mSensorList.lock();
    sensor->isCharged(false);
    updateKnownFile();
    mSensorList.unlock();
}

void EventHandler::onAckActivateCode() {
    transmitter->mTransmit.lock();
    if(transmitter->getTransmittingCode() == activateSirenCode) {
        transmitter->isAckReceived(true);
        transmitter->mTransmit.unlock();
        transmitter->startTransmitting.notify_all();
    }
    else
        transmitter->mTransmit.unlock();
}

void EventHandler::onAckDeactivateCode() {
    transmitter->mTransmit.lock();
    if(transmitter->getTransmittingCode() == deactivateSirenCode) {
        transmitter->isAckReceived(true);
        transmitter->mTransmit.unlock();
        transmitter->startTransmitting.notify_all();
    }
    else
        transmitter->mTransmit.unlock();
}

void EventHandler::onTamperActiveCode() {
    transmitter->mTransmit.lock();
    defensesActivated = true;
    transmitter->addTransmittingCode(ackControlUnitCode, SEND_ACK);
    transmitter->mTransmit.unlock();
    transmitter->startTransmitting.notify_all();
}

void EventHandler::onUnknownCode(code codeReceived) {
    cout << "EventHandler - codeReceived: " << codeReceived << endl;
    cout << "EventHandler - registerCode?: " << registerCode << endl;
    if (registerCode)
    {
        newCode = codeReceived;
        cout << "EventHandler - newCode: " << newCode << endl;
        codeArrived = true;
        statical::sharedCondition.notify_all();
    }
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
