#include "Core.h"

Core::Core() : receiver(PIN), eventHandler(&receiver, &transmitter, &knownSensorList, &codeMap)
{
    setupKnownSensors();
    receiverThread = thread(&Receiver::startReceiving, &receiver);
    transmitterThread = thread(&Transmitter::startTransmittingProtocol, &transmitter);
    eventHandlerThread = thread(&EventHandler::startListening, &eventHandler);
};

//TEST
//TODO
void Core::setupKnownSensors()
{
    ifstream readingFile(KNOWN_PATH);
    string line;
    if (readingFile.is_open())
    {
        while (getline(readingFile, line))
        {
            istringstream streamString(line);
            string field;
            getline(streamString, field, ';');
            switch (stoi(field))
            {
            case DOOR_SENSOR:
                DoorSensor *ds = new DoorSensor();
                getline(streamString, field, ';');
                ds->setSensorID(stoi(field));
                getline(streamString, field, ';');
                ds->setSensorState((State)stoi(field));
                getline(streamString, field, ';');
                ds->isEnabled((bool)stoi(field));
                getline(streamString, field, ';');
                ds->setOpenCode(stoi(field));
                getline(streamString, field, ';');
                ds->setCloseCode(stoi(field));
                getline(streamString, field);
                ds->setSensorName(field);
                knownSensorList.push_back(ds);
                codeMap[ds->getOpenCode()] = new pair<Action, Sensor *>(OPEN, ds);
                codeMap[ds->getCloseCode()] = new pair<Action, Sensor *>(CLOSE, ds);
                break;
            }
        }
        readingFile.close();
    }
    else
    {
        ofstream createdFile(KNOWN_PATH);
        createdFile.close();
    }
};

//TEST
//Return true if sensor added, false otherwise
bool Core::addSensorToList(Sensor *s)
{
    list<Sensor *>::iterator it = std::find_if(knownSensorList.begin(), knownSensorList.end(), [s](Sensor *sensor) { return sensor->getSensorID() == s->getSensorID(); });
    if (it == knownSensorList.end())
    {
        knownSensorList.push_back(s);
        return true;
    }
    return false;
};
//TEST
//Return true if sensor removed, false otherwise
bool Core::removeSensorFromList(Sensor *s)
{
    int listSize = knownSensorList.size();
    knownSensorList.remove_if([s](Sensor *sensor) { return sensor->getSensorID() == s->getSensorID(); });
    if (listSize != knownSensorList.size())
        return true;
    return false;
};
//TEST
//Return true if alarm is activatable, false otherwise
bool Core::isAlarmReady()
{
    eventHandler.mSensorList.lock();
    list<Sensor *>::iterator it = std::find_if(knownSensorList.begin(), knownSensorList.end(), [](Sensor *s) { return !s->isSensorReady(); });
    if (it == knownSensorList.end())
    {
        eventHandler.mSensorList.unlock();
        return true;
    }
    eventHandler.mSensorList.unlock();
    return false;
}

//??
int Core::getNewSensorID()
{
    if (knownSensorList.empty())
        return 0;
    return (*(--knownSensorList.end()))->getSensorID() + 1;
}

void Core::registerNewDoorSensor(TCPComm &tcpComm)
{
    DoorSensor *ds = new DoorSensor();
    ds->setSensorID(getNewSensorID());
    ds->setSensorState(OPENED);


    //cambia var

    eventHandler.registerCode = true;

    unique_lock<mutex> registerLock(statical::mSharedCondition);
    bool notTimedOut = statical::sharedCondition.wait_for(registerLock, chrono::seconds(30), [this, &tcpComm] {
        return (bool)eventHandler.codeArrived || tcpComm.isAvailable();
    });
    
    if (tcpComm.isAvailable())
    {
        string message = tcpComm.getMessage();
        cout << "Abort...";
        eventHandler.registerCode = false;
        delete ds;
        return;
    }
    if (!notTimedOut)
    {
        cout << "Timed out...";
        tcpComm.flush();
        tcpComm.sendMessage(Message::TIME_OUT);
        eventHandler.registerCode = false;
        delete ds;
        return;
    }

    eventHandler.codeArrived = false;
    code closeCode = eventHandler.newCode;
    ds->setCloseCode(closeCode);
    cout << "Core - closeCode: " << closeCode << endl;

    tcpComm.sendMessage(Message::NEXT_CODE);

    bool go = true;
    while (go)
    {
        notTimedOut = statical::sharedCondition.wait_for(registerLock, chrono::seconds(30), [this, &tcpComm] {
            return ((bool)eventHandler.codeArrived) || tcpComm.isAvailable();
        });

        if (tcpComm.isAvailable())
        {
            tcpComm.getMessage();
            cout << "Abort...";
            eventHandler.registerCode = false;
            delete ds;
            return;
        }
        if (!notTimedOut)
        {
            cout << "Timed out...";
            tcpComm.flush();
            tcpComm.sendMessage(Message::TIME_OUT);
            eventHandler.registerCode = false;
            delete ds;
            return;
        }
        eventHandler.codeArrived = false;
        code openCode = eventHandler.newCode;
        if (openCode != closeCode)
        {
            ds->setOpenCode(openCode);
            tcpComm.sendMessage(MessageType::STRING);
            cout << "Core - openCode: " << openCode << endl;
            go = false;
        }
    }
    eventHandler.registerCode = false;


    notTimedOut = statical::sharedCondition.wait_for(registerLock, chrono::seconds(30), [this, &tcpComm] {
        return tcpComm.isAvailable();
    });
    
    try
    {
        if (tcpComm.isAvailable())
        {
            string message = tcpComm.getMessage();
            cout << message << endl;
            if(message == MessageType::STRING) {
                tcpComm.sendMessage(Message::ACK);
                statical::sharedCondition.wait(registerLock, [this, &tcpComm] {
                    return tcpComm.isAvailable();
                });
                string sensorName = tcpComm.getMessage();
                cout << "SENSOR NAME: " << sensorName << endl;
                if (sensorName != Message::FAIL && sensorName != Message::ABORT && sensorName != MessageType::STRING)
                {
                    ds->setSensorName(sensorName);
                    ds->isEnabled(true);
                    eventHandler.mSensorList.lock();
                    addSensorToList(ds);
                    eventHandler.mSensorList.unlock();
                    eventHandler.mFile.lock();
                    ofstream out(KNOWN_PATH, ios::app);
                    codeMap[ds->getOpenCode()] = new pair<Action, Sensor *>(OPEN, ds);
                    codeMap[ds->getCloseCode()] = new pair<Action, Sensor *>(CLOSE, ds);
                    ds->writeToFile(out);
                    out.close();
                    eventHandler.mFile.unlock();
                    cout << "Core - sensorName" << endl;
                }
                else 
                    throw UnexpectedMessageException("Messaggio ricevuto dall'applicazione inaspettato");
            }
            else if(message == Message::FAIL || message == Message::ABORT) {
                cout << "Abort..." <<endl;
                delete ds;
                return;
            }
            else 
                throw UnexpectedMessageException("Messaggio ricevuto dall'applicazione inaspettato");       
        }
        else 
        {
            cout << "Timed out...";
            tcpComm.sendMessage(Message::TIME_OUT);
            eventHandler.registerCode = false;
            delete ds;
            return;
        }
    }
    catch (RegisterNewSensorException &e)
    {
        cout << e.what() << endl;
        tcpComm.sendMessage(Message::REGISTER_FAILED);
        delete ds;
        return;
    }

    tcpComm.sendMessage(Message::REGISTER_SUCCESS);
    cout << "Core - FINE" << endl;
}




void Core::activateAlarm(TCPComm &tcpComm)
{
    if (isAlarmReady() && !eventHandler.alarmActivated && !eventHandler.defensesActivated)
    {
        eventHandler.alarmActivated = true;
        tcpComm.sendMessage(Message::ACTIVATION_SUCCESS);
    }
    else
        tcpComm.sendMessage(Message::ACTIVATION_FAILED);
}

void Core::deactivateAlarm(TCPComm &tcpComm)
{
    if (eventHandler.alarmActivated || eventHandler.defensesActivated)
    {
        eventHandler.alarmActivated = false;
        if (eventHandler.defensesActivated)
        {
            transmitter.mTransmit.lock();
            transmitter.addTransmittingCode(deactivateSirenCode, WAIT_FOR_ACK);
            transmitter.mTransmit.unlock();
            transmitter.startTransmitting.notify_all();
            eventHandler.defensesActivated = false;
        }
        tcpComm.sendMessage(Message::DEACTIVATION_SUCCESS);
    }
    else
    {
        tcpComm.sendMessage(Message::DEACTIVATION_FAILED);
    }
}

void Core::sensorList(TCPComm &tcpComm)
{
    eventHandler.mSensorList.lock();
    for (Sensor *s : knownSensorList)
        tcpComm.sendMessage(s->getSensorInfo());
    eventHandler.mSensorList.unlock();
    tcpComm.sendMessage(Message::END_SENSOR_LIST);
}

void Core::deactivateSensor(TCPComm &tcpComm, string message)
{
    int sensorID = stoi(message.substr(0, message.length() - Message::DEACTIVATE_SENSOR.length() - SEPARATOR.length()));
    eventHandler.mSensorList.lock();
    list<Sensor *>::iterator it = std::find_if(knownSensorList.begin(), knownSensorList.end(), [sensorID](Sensor *sensor) { return sensor->getSensorID() == sensorID; });
    try
    {
        if (it != knownSensorList.end())
        {
            if ((*it)->isEnabled())
            {
                (*it)->isEnabled(false);
                eventHandler.updateKnownFile();
                tcpComm.sendMessage(Message::DEACTIVATE_SENSOR_SUCCESS);
            }
            else
                throw SensorAlreadyDisabledException("Sensore già disabilitato!");
        }
        else
        {
            throw EnabledSensorNotFoundException("Sensore non trovato!");
        }
    }
    catch (DisableSensorException &e)
    {
        cout << e.what() << endl;
        tcpComm.sendMessage(Message::DEACTIVATE_SENSOR_FAILED);
    }
    eventHandler.mSensorList.unlock();
}

void Core::activateSensor(TCPComm &tcpComm, string message)
{
    int sensorID = stoi(message.substr(0, message.length() - Message::ACTIVATE_SENSOR.length() - SEPARATOR.length()));
    eventHandler.mSensorList.lock();
    list<Sensor *>::iterator it = std::find_if(knownSensorList.begin(), knownSensorList.end(), [sensorID](Sensor *sensor) { return sensor->getSensorID() == sensorID; });
    try
    {
        if (it != knownSensorList.end())
        {
            if (!(*it)->isEnabled())
            {
                if (eventHandler.alarmActivated && (*it)->getSensorState() == OPENED)
                {
                    throw SensorOpenedException("Sensore aperto con allarme attivo!");
                }
                (*it)->isEnabled(true);
                eventHandler.updateKnownFile();
                tcpComm.sendMessage(Message::ACTIVATE_SENSOR_SUCCESS);
            }
            else
                throw SensorAlreadyEnabledException("Sensore già abilitato!");
        }
        else
        {
            throw DisabledSensorNotFoundException("Sensore non trovato!");
        }
    }
    catch (EnableSensorException &e)
    {
        cout << e.what() << endl;
        tcpComm.sendMessage(Message::ACTIVATE_SENSOR_FAILED);
    }
    eventHandler.mSensorList.unlock();
}

void Core::removeSensor(TCPComm &tcpComm, string message)
{
    int sensorID = stoi(message.substr(0, message.length() - Message::ACTIVATE_SENSOR.length() - SEPARATOR.length()));
    eventHandler.mSensorList.lock();
    list<Sensor *>::iterator it = std::find_if(knownSensorList.begin(), knownSensorList.end(), [sensorID](Sensor *sensor) { return sensor->getSensorID() == sensorID; });

    try
    {
        if (it != knownSensorList.end())
        {
            for (code sensorCode : (*it)->getCodeList())
                codeMap.erase(sensorCode);
            knownSensorList.erase(it);
            eventHandler.updateKnownFile();
            tcpComm.sendMessage(Message::REMOVE_SENSOR_SUCCESS);
        }
        else
        {
            throw SensorNotFoundException("Sensore non trovato!");
        }
    }
    catch (RemoveSensorException &e)
    {
        cout << e.what() << endl;
        tcpComm.sendMessage(Message::REMOVE_SENSOR_FAILED);
    }
    eventHandler.mSensorList.unlock();
}
