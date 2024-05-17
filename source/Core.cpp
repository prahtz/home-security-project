#include "Core.h"

Core::Core() : receiver(), eventHandler(&receiver, &transmitter, &firebaseMessagesHandler)
{

}

void Core::startService()
{
    Logger::log("Service started");
    critical_section::sensorsHandler->setupKnownSensors();
    critical_section::firebaseTokensHandler->setupTokenList();
    receiverThread = thread(&Receiver::startReceiving, &receiver);
    transmitterThread = thread(&Transmitter::startTransmittingProtocol, &transmitter);
    firebaseMessagesHandlerThread = thread(&FirebaseMessagesHandler::startService, &firebaseMessagesHandler);
    eventHandlerThread = thread(&EventHandler::startListening, &eventHandler);
}

bool Core::isAlarmReady()
{
    return critical_section::sensorsHandler.with_lock<bool>([](SensorsHandler& sensorsHandler) {
        list<Sensor*> sensorList = sensorsHandler.getSensorList();
        list<Sensor *>::iterator it = std::find_if(sensorList.begin(), sensorList.end(), [](Sensor *s) { return !s->isSensorReady(); });
        if (it == sensorList.end())
            return true;
        return false;
    });
}

void Core::registerNewDoorSensor(TCPComm &tcpComm)
{
    Logger::log("Registering new door sensor...");
    DoorSensor *ds = new DoorSensor();
    ds->setSensorID(critical_section::sensorsHandler->getNewSensorID());
    ds->setSensorState(OPENED);
    ds->isCharged(true);
    atomic<bool> abort = false, ack = false;
    string sensorName;
    Subscription<string>& sub = tcpComm.getMessageStream().listen();
    sub.onData([this, &sub, &tcpComm, &abort, &sensorName, &ack](string message) {
        cout << "Register SUB: " << message << endl;
        if (message == message::ABORT || message == message::FAIL)
        {
            abort = true;
            statical::sharedCondition.notify_all();
            sub.cancel();
        }
        else if (message == message::STRING_REQUEST)
        {
            ack = true;
            tcpComm.sendMessage(message::ACK);
        }
        else if (ack && message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            sensorName = message;
            statical::sharedCondition.notify_all();
            sub.cancel();
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
        sub.cancel();
        tcpComm.sendMessage(message::TIME_OUT);
        eventHandler.registerCode = false;
        delete ds;
        return;
    }
    catch (RegisterNewSensorException &e)
    {
        cout << e.what() << endl;
        tcpComm.sendMessage(message::REGISTER_FAILED);
        delete ds;
        return;
    }
    Logger::log(ds->getSensorName() + " sensor registered with open code " + to_string(ds->getOpenCode()) + " and close code " + to_string(ds->getCloseCode()));
    tcpComm.sendMessage(message::REGISTER_SUCCESS);
}

void Core::registerCloseCode(TCPComm &tcpComm, DoorSensor *ds, atomic<bool> &abort)
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
    tcpComm.sendMessage(message::NEXT_CODE);
}

void Core::registerOpenCode(TCPComm &tcpComm, DoorSensor *ds, atomic<bool> &abort)
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
            tcpComm.sendMessage(message::STRING_REQUEST);
            go = false;
        }
    }
    eventHandler.registerCode = false;
}

void Core::registerSensorName(TCPComm &tcpComm, DoorSensor *ds, atomic<bool> &abort, string &sensorName)
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
    ds->isEnabled(true);
    ds->isCharged(true);
    critical_section::sensorsHandler->addSensorToList(ds);
}

void Core::activateAlarm(TCPComm &tcpComm)
{
    if (isAlarmReady() && !eventHandler.alarmActivated && !eventHandler.defensesActivated)
    {
        eventHandler.alarmActivated = true;
        tcpComm.sendMessage(message::ACTIVATION_SUCCESS);
        Logger::log("Alarm activated");
    }
    else
    {
        tcpComm.sendMessage(message::ACTIVATION_FAILED);
        Logger::log("Alarm activation failed");
    }
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
        tcpComm.sendMessage(message::DEACTIVATION_SUCCESS);
        Logger::log("Alarm deactivated");
    }
    else
    {
        tcpComm.sendMessage(message::DEACTIVATION_FAILED);
        Logger::log("Alarm deactivation failed");
    }
}

void Core::sensorList(TCPComm &tcpComm)
{
    critical_section::sensorsHandler.with_lock<void>([&tcpComm](SensorsHandler &sensorsHandler) {
        list<Sensor*>& sensorList = sensorsHandler.getSensorList();
        for (Sensor *s : sensorList)
            tcpComm.sendMessage(message::STRING + s->getSensorInfo());
        tcpComm.sendMessage(message::END_SENSOR_LIST);
    });
}

void Core::deactivateSensor(TCPComm &tcpComm)
{
    Subscription<string>& sub = tcpComm.getMessageStream().listen();
    sub.onData([this, &tcpComm, &sub](string message) {
        cout << "Deact SUB: " << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            mCore.lock();
            int sensorID = stoi(message.substr(0, message.length() - message::DEACTIVATE_SENSOR.length() - SEPARATOR.length()));

            critical_section::sensorsHandler.with_lock<void>([sensorID, &tcpComm](SensorsHandler &sensorsHandler) {
                list<Sensor *>& sensorList = sensorsHandler.getSensorList();
                list<Sensor *>::iterator it = std::find_if(
                    sensorList.begin(), 
                    sensorList.end(), 
                    [sensorID](Sensor *sensor) { return sensor->getSensorID() == sensorID; }
                );
                try
                {
                    if (it != sensorList.end())
                    {
                        if ((*it)->isEnabled())
                        {
                            (*it)->isEnabled(false);
                            sensorsHandler.updateKnownFile();
                            tcpComm.sendMessage(message::DEACTIVATE_SENSOR_SUCCESS);
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
                    tcpComm.sendMessage(message::DEACTIVATE_SENSOR_FAILED);
                }
            });
            mCore.unlock();
            sub.cancel();
        }
        else if (message == message::FAIL)
            sub.cancel();
    });
    tcpComm.sendMessage(message::ACK);
}

void Core::activateSensor(TCPComm &tcpComm)
{
    Subscription<string>& sub = tcpComm.getMessageStream().listen();
    sub.onData([this, &tcpComm, &sub](string message) {
        cout << "Act SUB: " << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            mCore.lock();
            int sensorID = stoi(message.substr(0, message.length() - message::ACTIVATE_SENSOR.length() - SEPARATOR.length()));
            critical_section::sensorsHandler.with_lock<void>([this, sensorID, &tcpComm](SensorsHandler &sensorsHandler) {
                list<Sensor *>& sensorList = sensorsHandler.getSensorList();
                list<Sensor *>::iterator it = std::find_if(
                    sensorList.begin(), 
                    sensorList.end(), 
                    [sensorID](Sensor *sensor) { return sensor->getSensorID() == sensorID; }
                );
                try
                {
                    if (it != sensorList.end())
                    {
                        if (!(*it)->isEnabled())
                        {
                            if (eventHandler.alarmActivated && (*it)->getSensorState() == OPENED)
                            {
                                throw SensorOpenedException("Sensore aperto con allarme attivo!");
                            }
                            (*it)->isEnabled(true);
                            sensorsHandler.updateKnownFile();
                            tcpComm.sendMessage(message::ACTIVATE_SENSOR_SUCCESS);
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
                    tcpComm.sendMessage(message::ACTIVATE_SENSOR_FAILED);
                }
            });
            mCore.unlock();
            sub.cancel();
        }
        else if (message == message::FAIL)
            sub.cancel();
    });
    tcpComm.sendMessage(message::ACK);
}

void Core::removeSensor(TCPComm &tcpComm)
{
    if(!pinCheck(tcpComm))
        return;
    Subscription<string>& sub = tcpComm.getMessageStream().listen();
    sub.onData([this, &tcpComm, &sub](string message) {
        cout << "Remove SUB: " << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            mCore.lock();
            int sensorID = stoi(message.substr(0, message.length() - message::ACTIVATE_SENSOR.length() - SEPARATOR.length()));
            try
            {
                bool removed = critical_section::sensorsHandler->removeSensorFromList(sensorID);
                if (removed)
                {
                    tcpComm.sendMessage(message::REMOVE_SENSOR_SUCCESS);
                    Logger::log("Sensor " + std::to_string(sensorID) + " removed");
                }
                else
                {
                    throw SensorNotFoundException("Sensore non trovato!");
                }
            }
            catch (RemoveSensorException &e)
            {
                cout << e.what() << endl;
                tcpComm.sendMessage(message::REMOVE_SENSOR_FAILED);
            }
            mCore.unlock();
            sub.cancel();
        }
        else if (message == message::FAIL)
            sub.cancel();
    });
    tcpComm.sendMessage(message::ACK);
}

void Core::updateBattery(TCPComm &tcpComm)
{
    Subscription<string>& sub = tcpComm.getMessageStream().listen();
    sub.onData([this, &tcpComm, &sub](string message) {
        cout << "Updt SUB: " << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            mCore.lock();
            int sensorID = stoi(message.substr(0, message.length() - message::UPDATE_BATTERY.length() - SEPARATOR.length()));
            critical_section::sensorsHandler.with_lock<void>([sensorID, &tcpComm](SensorsHandler &sensorsHandler) {
                list<Sensor *>& sensorList = sensorsHandler.getSensorList();
                list<Sensor *>::iterator it = std::find_if(
                    sensorList.begin(), 
                    sensorList.end(), 
                    [sensorID](Sensor *sensor) { return sensor->getSensorID() == sensorID; }
                );
                try
                {
                    if (it != sensorList.end())
                    {
                        if (!(*it)->isCharged())
                        {
                            (*it)->isCharged(true);
                            sensorsHandler.updateKnownFile();
                            tcpComm.sendMessage(message::UPDATE_BATTERY_SUCCESS);
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
                    tcpComm.sendMessage(message::UPDATE_BATTERY_FAILED);
                }
            });
            mCore.unlock();
            sub.cancel();
        }
        else if (message == message::FAIL)
            sub.cancel();
    });
    tcpComm.sendMessage(message::ACK);
}

void Core::handleFirebaseToken(TCPComm &tcpComm)
{
    Subscription<string>& sub = tcpComm.getMessageStream().listen();
    sub.onData([this, &sub, &tcpComm](string token) {
        cout << "FIREBASE SUB: " << token << endl;
        if (token.find(message::STRING) != string::npos)
        {
            token = message::clear_string_message(token);
            critical_section::firebaseTokensHandler.with_lock<void>([token](FirebaseTokensHandler &firebaseTokensHandler) {
                list<string> &tokenList = firebaseTokensHandler.getTokenList();
                if (std::find(tokenList.begin(), tokenList.end(), token) == tokenList.end())
                {
                    cout << "Adding Token: " << token << endl;
                    tokenList.push_back(token);
                    firebaseTokensHandler.updateTokenList();
                }
            });
            
            tcpComm.sendMessage(message::FIREBASE_TOKEN_RECEIVED);
            Logger::log("New Firebase token received");
            sub.cancel();
        }
        else if (token == message::FAIL)
            sub.cancel();
    });
    tcpComm.sendMessage(message::ACK);
}

void Core::setupNewPIN(TCPComm &tcpComm)
{
    Logger::log("Setup new PIN...");
    string pin;

    atomic<bool> abort = false, update = false, success = false;
    Subscription<string>& sub = tcpComm.getMessageStream().listen();
    sub.onData([this, &sub, &tcpComm, &abort, &update, &success, &pin](string message) {
        cout << "PIN Updt sub:" + message << endl;
        if (message == message::ABORT || message == message::FAIL)
        {
            abort = true;
            statical::sharedCondition.notify_all();
            sub.cancel();
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
                sub.cancel();
            }
            else
            {
                if (pin != message)
                    tcpComm.sendMessage(message::PIN_CHECK_FAILED);
                else
                {
                    tcpComm.sendMessage(message::PIN_CHECK_SUCCESS);
                    update = true;
                    statical::sharedCondition.notify_all();
                }
            }
        }
        else if (message == message::FAIL)
            sub.cancel();
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
        tcpComm.sendMessage(message::ACK);
        waitOnCondition(abort, update);
        waitOnCondition(abort, success);
    }
    catch (PinNotFoundException e)
    {
        cout << e.what() << endl;
        sub.cancel();
        tcpComm.sendMessage(message::UPDATE_PIN_FAILED);
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
        sub.cancel();
        tcpComm.sendMessage(message::TIME_OUT);
        return;
    }
    Logger::log("New PIN set");
    tcpComm.sendMessage(message::UPDATE_PIN_SUCCESS);
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

bool Core::pinCheck(TCPComm &tcpComm)
{
    atomic<bool> abort = false, success = false;
    Subscription<string>& sub = tcpComm.getMessageStream().listen();
    sub.onData([this, &sub, &tcpComm, &abort, &success](string message) {
        cout << "PIN check SUB:" << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            std::string pin;
            ifstream in(PIN_PATH);
            in >> pin;
            if (pin != message)
                tcpComm.sendMessage(message::PIN_CHECK_FAILED);
            else {
                tcpComm.sendMessage(message::PIN_CHECK_SUCCESS);
                success = true;
                statical::sharedCondition.notify_all();
                sub.cancel();
            }
            in.close();
        }
        else if (message == message::ABORT || message == message::FAIL) {
            abort = true;
            statical::sharedCondition.notify_all();
            sub.cancel();
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
        tcpComm.sendMessage(message::ACK);
        waitOnCondition(abort, success);
    }
    catch(AbortException e) {
        cout << e.what() << endl;
        sub.cancel();
        return false;
    }
    catch(TimeOutException e) {
        cout << e.what() << endl;
        tcpComm.sendMessage(message::TIME_OUT);
        sub.cancel();
        return false;
    }
    catch(PinNotFoundException e) {
        cout << e.what() << endl;
        tcpComm.sendMessage(message::PIN_CHECK_FAILED);
        sub.cancel();
        return false;
    }
    Logger::log("PIN check success");
    return true;
}

void Core::setupFirstPIN(TCPComm &tcpComm)
{
    atomic<bool> abort = false, success = false;
    Subscription<string>& sub = tcpComm.getMessageStream().listen();
    sub.onData([this, &sub, &tcpComm, &abort, &success](string message) {
        cout << "PIN first setup SUB:" << message << endl;
        if (message.find(message::STRING) != string::npos)
        {
            message = message::clear_string_message(message);
            ofstream out(PIN_PATH, ios::trunc);
            out << message;
            out.close();
            success = true;
            statical::sharedCondition.notify_all();
            sub.cancel();
        }
        else if (message == message::ABORT || message == message::FAIL) {
            abort = true;
            statical::sharedCondition.notify_all();
            sub.cancel();
        }
    });
    try {
        if (std::filesystem::exists(PIN_PATH))
        {
            ifstream in(PIN_PATH);
            if (in.peek() != std::ifstream::traits_type::eof()) {
                tcpComm.sendMessage(message::PIN_FIRST_SETUP_FAILED);
                sub.cancel();
                in.close();
                return;
            }
            in.close();
        }
        tcpComm.sendMessage(message::ACK);
        waitOnCondition(abort, success);
    }
    catch(AbortException e) {
        cout << e.what() << endl;
        sub.cancel();
        return;
    }
    catch(TimeOutException e) {
        cout << e.what() << endl;
        tcpComm.sendMessage(message::TIME_OUT);
        sub.cancel();
        return;
    }
    tcpComm.sendMessage(message::PIN_FIRST_SETUP_SUCCESS);
    Logger::log("PIN set for the first time");
}
