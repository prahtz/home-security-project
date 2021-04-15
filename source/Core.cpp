#include "Core.h"
list<string> Core::tokenList;

Core::Core() : receiver(), eventHandler(&receiver, &transmitter, &firebaseMessagesHandler, &knownSensorList, &tokenList, &codeMap)
{

}

void Core::startService()
{
    Logger::log("Service started");
    setupKnownSensors();
    setupTokenList();
    receiverThread = thread(&Receiver::startReceiving, &receiver);
    transmitterThread = thread(&Transmitter::startTransmittingProtocol, &transmitter);
    firebaseMessagesHandlerThread = thread(&FirebaseMessagesHandler::startService, &firebaseMessagesHandler);
    eventHandlerThread = thread(&EventHandler::startListening, &eventHandler);
}

void Core::updateCodeMap(DoorSensor *ds)
{
    codeMap[ds->getOpenCode()] = new pair<Action, Sensor *>(OPEN, ds);
    codeMap[ds->getCloseCode()] = new pair<Action, Sensor *>(CLOSE, ds);
    codeMap[ds->getBatteryLowCode()] = new pair<Action, Sensor *>(BATTERY_LOW, ds);
}

void Core::setupKnownSensors()
{
    std::filesystem::create_directory(PATH);
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
                ds->setBatteryLowCode(min(ds->getOpenCode(), ds->getCloseCode()));
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
    Logger::log("Registering new door sensor...");
    DoorSensor *ds = new DoorSensor();
    ds->setSensorID(getNewSensorID());
    ds->setSensorState(OPENED);
    ds->isCharged(true);
    atomic<bool> abort = false, ack = false;
    string sensorName;
    Subscription<string> *sub = tcpComm->getMessageStream()->listen();
    sub->onData([this, sub, &tcpComm, &abort, &sensorName, &ack](string message) {
        cout << "Register SUB: " << message << endl;
        if (message == message::ABORT || message == message::FAIL)
        {
            abort = true;
            statical::sharedCondition.notify_all();
            sub->cancel();
        }
        else if (message == message::STRING_REQUEST)
        {
            ack = true;
            tcpComm->sendMessage(message::ACK);
        }
        else if (ack && message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            sensorName = message;
            statical::sharedCondition.notify_all();
            sub->cancel();
        }
    });

    try
    {
        registerCloseCode(tcpComm, ds, abort);
        registerOpenCode(tcpComm, ds, abort);
        registerSensorName(tcpComm, ds, abort, sensorName);
    }
    catch (AbortException &e)
    {
        cout << e.what() << endl;
        eventHandler.registerCode = false;
        delete ds;
        return;
    }
    catch (TimeOutException &e)
    {
        cout << e.what() << endl;
        sub->cancel();
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
    Logger::log(ds->getSensorName() + " sensor registered with open code " + to_string(ds->getOpenCode()) + " and close code " + to_string(ds->getCloseCode()));
    tcpComm->sendMessage(message::REGISTER_SUCCESS);
}

void Core::registerCloseCode(TCPComm *tcpComm, DoorSensor *ds, atomic<bool> &abort)
{
    eventHandler.registerCode = true;
    unique_lock<mutex> registerLock(statical::mSharedCondition);
    bool notTimedOut = statical::sharedCondition.wait_for(registerLock, chrono::seconds(30), [this, &tcpComm, &abort] {
        return (bool)eventHandler.codeArrived || abort;
    });

    if (abort)
        throw AbortException("Register sensor abort, first step.");
    else if (!notTimedOut)
        throw TimeOutException("Register sensor timed out, first step");

    eventHandler.codeArrived = false;
    code closeCode = eventHandler.newCode;
    ds->setCloseCode(closeCode);
    tcpComm->sendMessage(message::NEXT_CODE);
}

void Core::registerOpenCode(TCPComm *tcpComm, DoorSensor *ds, atomic<bool> &abort)
{
    bool go = true;
    code openCode;
    unique_lock<mutex> registerLock(statical::mSharedCondition);
    while (go)
    {
        bool notTimedOut = statical::sharedCondition.wait_for(registerLock, chrono::seconds(30), [this, &tcpComm, &abort] {
            return ((bool)eventHandler.codeArrived) || abort;
        });

        if (abort)
            throw AbortException("Register sensor abort, second step.");
        else if (!notTimedOut)
            throw TimeOutException("Register sensor timed out, second step");

        eventHandler.codeArrived = false;
        openCode = eventHandler.newCode;
        if (openCode != ds->getCloseCode())
        {
            ds->setOpenCode(openCode);
            tcpComm->sendMessage(message::STRING_REQUEST);
            go = false;
        }
    }
    eventHandler.registerCode = false;
}

void Core::registerSensorName(TCPComm *tcpComm, DoorSensor *ds, atomic<bool> &abort, string &sensorName)
{
    unique_lock<mutex> registerLock(statical::mSharedCondition);
    bool notTimedOut = statical::sharedCondition.wait_for(registerLock, chrono::seconds(30), [this, &tcpComm, &abort, &sensorName] {
        return !sensorName.empty() || abort;
    });

    if (abort)
        throw AbortException("Register sensor abort, third step.");
    else if (!notTimedOut)
        throw TimeOutException("Register sensor timed out, third step");

    ds->setSensorName(sensorName);
    ds->setBatteryLowCode(min(ds->getOpenCode(), ds->getCloseCode()));
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

void Core::activateAlarm(TCPComm *tcpComm)
{
    if (isAlarmReady() && !eventHandler.alarmActivated && !eventHandler.defensesActivated)
    {
        eventHandler.alarmActivated = true;
        tcpComm->sendMessage(message::ACTIVATION_SUCCESS);
        Logger::log("Alarm activated");
    }
    else
    {
        tcpComm->sendMessage(message::ACTIVATION_FAILED);
        Logger::log("Alarm activation failed");
    }
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
        Logger::log("Alarm deactivated");
    }
    else
    {
        tcpComm->sendMessage(message::DEACTIVATION_FAILED);
        Logger::log("Alarm deactivation failed");
    }
}

void Core::sensorList(TCPComm *tcpComm)
{
    eventHandler.mSensorList.lock();
    for (Sensor *s : knownSensorList)
        tcpComm->sendMessage(message::STRING + s->getSensorInfo());
    eventHandler.mSensorList.unlock();
    tcpComm->sendMessage(message::END_SENSOR_LIST);
}

void Core::deactivateSensor(TCPComm *tcpComm)
{
    Subscription<string> *sub = tcpComm->getMessageStream()->listen();
    sub->onData([this, tcpComm, sub](string message) {
        cout << "Deact SUB: " << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            mCore.lock();
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
                        Logger::log((*it)->getSensorInfo() + " deactivated");
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
            mCore.unlock();
            sub->cancel();
        }
        else if (message == message::FAIL)
            sub->cancel();
    });
    tcpComm->sendMessage(message::ACK);
}

void Core::activateSensor(TCPComm *tcpComm)
{
    Subscription<string> *sub = tcpComm->getMessageStream()->listen();
    sub->onData([this, tcpComm, sub](string message) {
        cout << "Act SUB: " << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            mCore.lock();
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
                        Logger::log((*it)->getSensorInfo() + " reactivated");
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
            mCore.unlock();
            sub->cancel();
        }
        else if (message == message::FAIL)
            sub->cancel();
    });
    tcpComm->sendMessage(message::ACK);
}

void Core::removeSensor(TCPComm *tcpComm)
{
    if(!pinCheck(tcpComm))
        return;
    Subscription<string> *sub = tcpComm->getMessageStream()->listen();
    sub->onData([this, tcpComm, sub](string message) {
        cout << "Remove SUB: " << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            mCore.lock();
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
                    Logger::log((*it)->getSensorInfo() + " sensor removed");
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
            mCore.unlock();
            sub->cancel();
        }
        else if (message == message::FAIL)
            sub->cancel();
    });
    tcpComm->sendMessage(message::ACK);
}

void Core::updateBattery(TCPComm *tcpComm)
{
    Subscription<string> *sub = tcpComm->getMessageStream()->listen();
    sub->onData([this, tcpComm, sub](string message) {
        cout << "Updt SUB: " << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            mCore.lock();
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
                        Logger::log((*it)->getSensorInfo() + " sensor recharged");
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
            mCore.unlock();
            sub->cancel();
        }
        else if (message == message::FAIL)
            sub->cancel();
    });
    tcpComm->sendMessage(message::ACK);
}

void Core::handleFirebaseToken(TCPComm *tcpComm)
{
    Subscription<string> *sub = tcpComm->getMessageStream()->listen();
    sub->onData([this, sub, tcpComm](string token) {
        cout << "FIREBASE SUB: " << token << endl;
        if (token.find(message::STRING) != string::npos)
        {
            token = message::clear_string_message(token);
            if (std::find(tokenList.begin(), tokenList.end(), token) == tokenList.end())
            {
                mCore.lock();
                FirebaseOperation *operation = new FirebaseOperation("add");
                operation->addRegID(token);
                cout << "Adding Token: " << token << endl;
                firebaseMessagesHandler.addMessage(operation);
                statical::newFirebaseNotification.notify_all();
                tokenList.push_back(token);
                updateTokenList();
                mCore.unlock();
            }
            tcpComm->sendMessage(message::FIREBASE_TOKEN_RECEIVED);
            Logger::log("New Firebase token received");
            sub->cancel();
        }
        else if (token == message::FAIL)
            sub->cancel();
    });
    tcpComm->sendMessage(message::ACK);
}

void Core::updateTokenList()
{
    ofstream out(TOKEN_PATH, ios::trunc);
    for (string s : tokenList)
        out << s << endl;
    out.close();
}

void Core::setupNewPIN(TCPComm *tcpComm)
{
    Logger::log("Setup new PIN...");
    string pin;

    atomic<bool> abort = false, update = false, success = false;
    Subscription<string> *sub = tcpComm->getMessageStream()->listen();
    sub->onData([this, sub, &tcpComm, &abort, &update, &success, &pin](string message) {
        cout << "PIN Updt sub:" + message << endl;
        if (message == message::ABORT || message == message::FAIL)
        {
            abort = true;
            statical::sharedCondition.notify_all();
            sub->cancel();
        }
        else if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            if (update)
            {
                ofstream out(PIN_PATH, ios::trunc);
                out << message;
                pin = message;
                success = true;
                out.close();
                statical::sharedCondition.notify_all();
                sub->cancel();
            }
            else
            {
                if (pin != message)
                    tcpComm->sendMessage(message::PIN_CHECK_FAILED);
                else
                {
                    tcpComm->sendMessage(message::PIN_CHECK_SUCCESS);
                    update = true;
                    statical::sharedCondition.notify_all();
                }
            }
        }
        else if (message == message::FAIL)
            sub->cancel();
    });
    try
    {
        if (std::filesystem::exists(PIN_PATH))
        {
            ifstream in(PIN_PATH);
            if (in.peek() != std::ifstream::traits_type::eof())
                in >> pin;
            else {
                in.close();
                throw PinNotFoundException("PIN non trovato");
            }
            in.close();
        }
        else
            throw PinNotFoundException("PIN non trovato");
        tcpComm->sendMessage(message::ACK);
        waitOnCondition(abort, update);
        waitOnCondition(abort, success);
    }
    catch (PinNotFoundException e)
    {
        cout << e.what() << endl;
        sub->cancel();
        tcpComm->sendMessage(message::UPDATE_PIN_FAILED);
        return;
    }
    catch (AbortException &e)
    {
        cout << e.what() << endl;
        return;
    }
    catch (TimeOutException &e)
    {
        cout << e.what() << endl;
        sub->cancel();
        tcpComm->sendMessage(message::TIME_OUT);
        return;
    }
    Logger::log("New PIN set");
    tcpComm->sendMessage(message::UPDATE_PIN_SUCCESS);
}

void Core::waitOnCondition(atomic<bool> &abort, atomic<bool> &cond)
{
    unique_lock<mutex> registerLock(statical::mSharedCondition);
    bool notTimedOut = statical::sharedCondition.wait_for(registerLock, chrono::seconds(30), [this, &abort, &cond] {
        return abort || cond;
    });
    if (abort)
        throw AbortException("Abort received");
    else if (!notTimedOut)
        throw TimeOutException("Timed out");
}

bool Core::pinCheck(TCPComm *tcpComm)
{
    atomic<bool> abort = false, success = false;
    Subscription<string> *sub = tcpComm->getMessageStream()->listen();
    sub->onData([this, sub, tcpComm, &abort, &success](string message) {
        cout << "PIN check SUB:" << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            std::string pin;
            ifstream in(PIN_PATH);
            in >> pin;
            if (pin != message)
                tcpComm->sendMessage(message::PIN_CHECK_FAILED);
            else {
                tcpComm->sendMessage(message::PIN_CHECK_SUCCESS);
                success = true;
                statical::sharedCondition.notify_all();
                sub->cancel();
            }
            in.close();
        }
        else if (message == message::ABORT || message == message::FAIL) {
            abort = true;
            statical::sharedCondition.notify_all();
            sub->cancel();
        }
    });
    try {
        if (std::filesystem::exists(PIN_PATH))
        {
            ifstream in(PIN_PATH);
            if (in.peek() == std::ifstream::traits_type::eof()) {
                in.close();
                throw PinNotFoundException("PIN non trovato");
            }
        }
        else
            throw PinNotFoundException("PIN non trovato");
        tcpComm->sendMessage(message::ACK);
        waitOnCondition(abort, success);
    }
    catch(AbortException e) {
        cout << e.what() << endl;
        sub->cancel();
        return false;
    }
    catch(TimeOutException e) {
        cout << e.what() << endl;
        tcpComm->sendMessage(message::TIME_OUT);
        sub->cancel();
        return false;
    }
    catch(PinNotFoundException e) {
        cout << e.what() << endl;
        tcpComm->sendMessage(message::PIN_CHECK_FAILED);
        sub->cancel();
        return false;
    }
    Logger::log("PIN check success");
    return true;
}

void Core::setupFirstPIN(TCPComm *tcpComm)
{
    atomic<bool> abort = false, success = false;
    Subscription<string> *sub = tcpComm->getMessageStream()->listen();
    sub->onData([this, sub, tcpComm, &abort, &success](string message) {
        cout << "PIN first setup SUB:" << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            ofstream out(PIN_PATH, ios::trunc);
            out << message;
            out.close();
            success = true;
            statical::sharedCondition.notify_all();
            sub->cancel();
        }
        else if (message == message::ABORT || message == message::FAIL) {
            abort = true;
            statical::sharedCondition.notify_all();
            sub->cancel();
        }
    });
    try {
        if (std::filesystem::exists(PIN_PATH))
        {
            ifstream in(PIN_PATH);
            if (in.peek() != std::ifstream::traits_type::eof()) {
                tcpComm->sendMessage(message::PIN_FIRST_SETUP_FAILED);
                sub->cancel();
                in.close();
                return;
            }
            in.close();
        }
        tcpComm->sendMessage(message::ACK);
        waitOnCondition(abort, success);
    }
    catch(AbortException e) {
        cout << e.what() << endl;
        sub->cancel();
        return;
    }
    catch(TimeOutException e) {
        cout << e.what() << endl;
        tcpComm->sendMessage(message::TIME_OUT);
        sub->cancel();
        return;
    }
    tcpComm->sendMessage(message::PIN_FIRST_SETUP_SUCCESS);
    Logger::log("PIN set for the first time");
}
