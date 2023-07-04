#include "EventHandler.h"

atomic<bool> EventHandler::alarmActivated(false);

EventHandler::EventHandler(Receiver *receiver, Transmitter *transmitter, FirebaseMessagesHandler *firebaseMessagesHandler)
{
    this->receiver = receiver;
    this->transmitter = transmitter;
    this->firebaseMessagesHandler = firebaseMessagesHandler;

    registerCode = false;
    codeArrived = false;
    alarmActivated = false;
    defensesActivated = false;
}

void EventHandler::startListening()
{
    while (true)
    {
        unique_lock<mutex> receiverLock(receiver->mBuff);
        receiver->codeAvailable.wait(receiverLock, [this] { return !receiver->isBufferEmpty(); });

        code codeReceived = receiver->popCodeFromBuffer();
        critical_section::sensorsHandler.with_lock<void>([this, codeReceived](SensorsHandler& sensorsHandler){
            map<code, pair<Action, Sensor *> *>& codeMap = sensorsHandler.getCodeMap();
            map<code, pair<Action, Sensor *> *>::iterator mapIterator = codeMap.find(codeReceived);
            bool sensorCode = codeMap.end() != mapIterator;
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
                        if(!isTransmittingCode(codeReceived))
                            onUnknownCode(codeReceived);
                }
            }
        });
    }
}

void EventHandler::onSensorOpen(Sensor *sensor)
{
    if (alarmActivated && sensor->isEnabled() && !defensesActivated) 
        activateDefenses();
    sensor->setSensorState(OPENED);
    critical_section::sensorsHandler->updateKnownFile();
    Logger::log(sensor->getSensorInfo() + ": received open code");
}

void EventHandler::onSensorClose(Sensor *sensor)
{
    sensor->setSensorState(CLOSED);
    critical_section::sensorsHandler->updateKnownFile();
    Logger::log(sensor->getSensorInfo() + ": received close code");
}

void EventHandler::onSensorBatteryLow(Sensor *sensor)
{
    sensor->isCharged(false);
    critical_section::sensorsHandler->updateKnownFile();
    Logger::log(sensor->getSensorInfo() + ": received battery low code");
}

void EventHandler::onAckActivateCode() {
    transmitter->mTransmit.lock();
    if(transmitter->getTransmittingCode() == activateSirenCode) {
        transmitter->isAckReceived(true);
        Logger::log("ACK message on the activate siren code received");
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
        Logger::log("ACK message on the deactivate siren code received");
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
    Logger::log("Tamper code received, sending ACK...");
    transmitter->mTransmit.unlock();
    transmitter->startTransmitting.notify_all();
}

void EventHandler::onUnknownCode(code codeReceived) {
    cout << "EventHandler - codeReceived: " << codeReceived << endl;
    cout << "EventHandler - registerCode?: " << registerCode << endl;
    Logger::log("Unknown code received: " + to_string(codeReceived));
    if (registerCode)
    {
        newCode = codeReceived;
        cout << "EventHandler - newCode: " << newCode << endl;
        codeArrived = true;
        statical::sharedCondition.notify_all();
    }
}

bool EventHandler::isTransmittingCode(code codeReceived) {
    return codeReceived == activateSirenCode || codeReceived == deactivateSirenCode || codeReceived == ackControlUnitCode
    || codeReceived == enableTamperCode;
}

void EventHandler::activateDefenses()
{
    Logger::log("Activating defenses!");
    defensesActivated = true;
    transmitter->mTransmit.lock();
    if(transmitter->getTransmittingCode() != activateSirenCode) {
        transmitter->addTransmittingCode(activateSirenCode, WAIT_FOR_ACK);
        transmitter->mTransmit.unlock();
        Logger::log("Starting transmission of activate siren code");
        transmitter->startTransmitting.notify_all();
    }
    else
        transmitter->mTransmit.unlock();
    
    critical_section::firebaseTokensHandler.with_lock<void>([this](FirebaseTokensHandler& firebaseTokensHandler){
        list<string> &tokenList = firebaseTokensHandler.getTokenList();
        for(string token : tokenList) {
            FirebaseNotification* notification = new FirebaseNotification();
            notification->setTitle("ALLARME ATTIVO");
            notification->setBody("Intrusione rilevata!");
            notification->setToken(token);
            firebaseMessagesHandler->addMessage(notification);
        }
    });
    
    statical::newFirebaseNotification.notify_all();
}

