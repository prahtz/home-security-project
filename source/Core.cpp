#include "Core.h"
list<string> Core::tokenList;

Core::Core() : receiver(), eventHandler(&receiver, &transmitter, &firebaseMessagesHandler, &knownSensorList, &tokenList, &codeMap)
{
    setupKnownSensors();
    setupTokenList();
    receiverThread = thread(&Receiver::startReceiving, &receiver);
    transmitterThread = thread(&Transmitter::startTransmittingProtocol, &transmitter);
    firebaseMessagesHandlerThread = thread(&FirebaseMessagesHandler::startService, &firebaseMessagesHandler);
    eventHandlerThread = thread(&EventHandler::startListening, &eventHandler);
};

void Core::updateCodeMap(DoorSensor *ds)
{
    codeMap[ds->getOpenCode()] = new pair<Action, Sensor *>(OPEN, ds);
    codeMap[ds->getCloseCode()] = new pair<Action, Sensor *>(CLOSE, ds);
    codeMap[ds->getBatteryLowCode()] = new pair<Action, Sensor *>(BATTERY_LOW, ds);
}

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
                ds->isCharged((bool)stoi(field));
                getline(streamString, field, ';');
                ds->setOpenCode(stoi(field));
                getline(streamString, field, ';');
                ds->setCloseCode(stoi(field));
                getline(streamString, field);
                ds->setSensorName(field);
                ds->setBatteryLowCode(ds->getOpenCode() - BATTERY_LOW_SHIFT);
                knownSensorList.push_back(ds);
                updateCodeMap(ds);
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

void Core::setupTokenList()
{
    ifstream readingFile(TOKEN_PATH);
    string line;
    if (readingFile.is_open())
    {
        while (getline(readingFile, line))
            tokenList.push_back(line);
        readingFile.close();
    }
    else
    {
        ofstream createdFile(TOKEN_PATH);
        createdFile.close();
    }
};

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

bool Core::removeSensorFromList(Sensor *s)
{
    int listSize = knownSensorList.size();
    knownSensorList.remove_if([s](Sensor *sensor) { return sensor->getSensorID() == s->getSensorID(); });
    if (listSize != knownSensorList.size())
        return true;
    return false;
};

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

int Core::getNewSensorID()
{
    if (knownSensorList.empty())
        return 0;
    return (*(--knownSensorList.end()))->getSensorID() + 1;
}

void Core::registerNewDoorSensor(TCPComm *tcpComm)
{
    DoorSensor *ds = new DoorSensor();
    ds->setSensorID(getNewSensorID());
    ds->setSensorState(OPENED);
    ds->isCharged(true);
    try
    {
        registerCloseCode(tcpComm, ds);
        registerOpenCode(tcpComm, ds);
        registerSensorName(tcpComm, ds);
    }
    catch (AbortException &e)
    {
        cout << e.what() << endl;
        tcpComm->flush();
        eventHandler.registerCode = false;
        delete ds;
        return;
    }
    catch (TimeOutException &e)
    {
        cout << e.what() << endl;
        tcpComm->flush();
        tcpComm->sendMessage(message::TIME_OUT);
        eventHandler.registerCode = false;
        delete ds;
        return;
    }
    catch (RegisterNewSensorException &e)
    {
        cout << e.what() << endl;
        tcpComm->sendMessage(message::REGISTER_FAILED);
        delete ds;
        return;
    }

    tcpComm->sendMessage(message::REGISTER_SUCCESS);
}

void Core::registerCloseCode(TCPComm *tcpComm, DoorSensor *ds)
{
    eventHandler.registerCode = true;
    unique_lock<mutex> registerLock(statical::mSharedCondition);
    bool notTimedOut = statical::sharedCondition.wait_for(registerLock, chrono::seconds(30), [this, &tcpComm] {
        return (bool)eventHandler.codeArrived || tcpComm->isAvailable();
    });

    if (tcpComm->isAvailable())
    {

        if (tcpComm->getMessage() == message::ABORT)
            throw AbortException("Register sensor abort, first step.");
        throw UnexpectedMessageException("Messaggio ricevuto dall'applicazione inaspettato, first step.");
    }
    if (!notTimedOut)
        throw TimeOutException("Register sensor timed out, first step");

    eventHandler.codeArrived = false;
    code closeCode = eventHandler.newCode;
    ds->setCloseCode(closeCode);

    tcpComm->sendMessage(message::NEXT_CODE);
}

void Core::registerOpenCode(TCPComm *tcpComm, DoorSensor *ds)
{
    bool go = true;
    code openCode;
    unique_lock<mutex> registerLock(statical::mSharedCondition);
    while (go)
    {
        bool notTimedOut = statical::sharedCondition.wait_for(registerLock, chrono::seconds(30), [this, &tcpComm] {
            return ((bool)eventHandler.codeArrived) || tcpComm->isAvailable();
        });

        if (tcpComm->isAvailable())
        {
            string m = tcpComm->getMessage();
            if (m == message::ABORT)
                throw AbortException("Register sensor abort, second step.");
            throw UnexpectedMessageException("Messaggio ricevuto dall'applicazione inaspettato, second step");
        }
        if (!notTimedOut)
            throw TimeOutException("Register sensor timed out, second step");

        eventHandler.codeArrived = false;
        openCode = eventHandler.newCode;
        if (openCode != ds->getCloseCode())
        {
            ds->setOpenCode(openCode);
            tcpComm->sendMessage(message::STRING);
            go = false;
        }
    }
    eventHandler.registerCode = false;
}

void Core::registerSensorName(TCPComm *tcpComm, DoorSensor *ds)
{
    unique_lock<mutex> registerLock(statical::mSharedCondition);
    bool notTimedOut = statical::sharedCondition.wait_for(registerLock, chrono::seconds(30), [this, &tcpComm] {
        return tcpComm->isAvailable();
    });

    if (tcpComm->isAvailable())
    {
        string message = tcpComm->getMessage();
        if (message == message::STRING)
        {
            tcpComm->sendMessage(message::ACK);
            statical::sharedCondition.wait(registerLock, [this, &tcpComm] {
                return tcpComm->isAvailable();
            });
            string sensorName = tcpComm->getMessage();
            if (sensorName != message::FAIL && sensorName != message::ABORT && sensorName != message::STRING)
            {
                ds->setSensorName(sensorName);
                ds->setBatteryLowCode(ds->getOpenCode() - BATTERY_LOW_SHIFT);
                ds->isEnabled(true);
                ds->isCharged(true);
                eventHandler.mSensorList.lock();
                addSensorToList(ds);
                eventHandler.mSensorList.unlock();
                eventHandler.mFile.lock();
                ofstream out(KNOWN_PATH, ios::app);
                updateCodeMap(ds);
                ds->writeToFile(out);
                out.close();
                eventHandler.mFile.unlock();
            }
            else
                throw UnexpectedMessageException("Messaggio ricevuto dall'applicazione inaspettato, third step, expected sensor name");
        }
        else if (message == message::ABORT)
        {
            throw AbortException("Register sensor abort, third step.");
        }
        else
            throw UnexpectedMessageException("Messaggio ricevuto dall'applicazione inaspettato, third step, expected STRING");
    }
    else
        throw TimeOutException("Register sensor timed out, third step");
}

void Core::activateAlarm(TCPComm *tcpComm)
{
    if (isAlarmReady() && !eventHandler.alarmActivated && !eventHandler.defensesActivated)
    {
        eventHandler.alarmActivated = true;
        tcpComm->sendMessage(message::ACTIVATION_SUCCESS);
    }
    else
        tcpComm->sendMessage(message::ACTIVATION_FAILED);
}

void Core::deactivateAlarm(TCPComm *tcpComm)
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
        tcpComm->sendMessage(message::DEACTIVATION_SUCCESS);
    }
    else
    {
        tcpComm->sendMessage(message::DEACTIVATION_FAILED);
    }
}

void Core::sensorList(TCPComm *tcpComm)
{
    eventHandler.mSensorList.lock();
    for (Sensor *s : knownSensorList)
        tcpComm->sendMessage(s->getSensorInfo());
    eventHandler.mSensorList.unlock();
    tcpComm->sendMessage(message::END_SENSOR_LIST);
}

void Core::deactivateSensor(TCPComm *tcpComm, string message)
{
    int sensorID = stoi(message.substr(0, message.length() - message::DEACTIVATE_SENSOR.length() - SEPARATOR.length()));
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
                tcpComm->sendMessage(message::DEACTIVATE_SENSOR_SUCCESS);
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
        tcpComm->sendMessage(message::DEACTIVATE_SENSOR_FAILED);
    }
    eventHandler.mSensorList.unlock();
}

void Core::activateSensor(TCPComm *tcpComm, string message)
{
    int sensorID = stoi(message.substr(0, message.length() - message::ACTIVATE_SENSOR.length() - SEPARATOR.length()));
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
                tcpComm->sendMessage(message::ACTIVATE_SENSOR_SUCCESS);
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
        tcpComm->sendMessage(message::ACTIVATE_SENSOR_FAILED);
    }
    eventHandler.mSensorList.unlock();
}

void Core::removeSensor(TCPComm *tcpComm, string message)
{
    int sensorID = stoi(message.substr(0, message.length() - message::ACTIVATE_SENSOR.length() - SEPARATOR.length()));
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
            tcpComm->sendMessage(message::REMOVE_SENSOR_SUCCESS);
        }
        else
        {
            throw SensorNotFoundException("Sensore non trovato!");
        }
    }
    catch (RemoveSensorException &e)
    {
        cout << e.what() << endl;
        tcpComm->sendMessage(message::REMOVE_SENSOR_FAILED);
    }
    eventHandler.mSensorList.unlock();
}

void Core::updateBattery(TCPComm *tcpComm, string message)
{
    int sensorID = stoi(message.substr(0, message.length() - message::UPDATE_BATTERY.length() - SEPARATOR.length()));
    eventHandler.mSensorList.lock();
    list<Sensor *>::iterator it = std::find_if(knownSensorList.begin(), knownSensorList.end(), [sensorID](Sensor *sensor) { return sensor->getSensorID() == sensorID; });

    try
    {
        if (it != knownSensorList.end())
        {
            if (!(*it)->isCharged())
            {
                (*it)->isCharged(true);
                eventHandler.updateKnownFile();
                tcpComm->sendMessage(message::UPDATE_BATTERY_SUCCESS);
            }
            else
            {
                throw SensorChargedException("Sensore non scarico!");
            }
        }
        else
        {
            throw UpdateBatterySensorNotFoundException("Sensore non trovato!");
        }
    }
    catch (UpdateBatteryException &e)
    {
        cout << e.what() << endl;
        tcpComm->sendMessage(message::UPDATE_BATTERY_FAILED);
    }
    eventHandler.mSensorList.unlock();
}

void Core::handleFirebaseToken(string token)
{
    if (std::find(tokenList.begin(), tokenList.end(), token) == tokenList.end())
    {
        FirebaseOperation *operation = new FirebaseOperation("add");
        operation->addRegID(token);
        cout << "Adding Token: " << token << endl;
        firebaseMessagesHandler.addMessage(operation);
        statical::newFirebaseNotification.notify_all();
        tokenList.push_back(token);
        updateTokenList();
    }
}

void Core::updateTokenList()
{
    ofstream out(TOKEN_PATH, ios::trunc);
    for (string s : tokenList)
        out << s << endl;
    out.close();
}
