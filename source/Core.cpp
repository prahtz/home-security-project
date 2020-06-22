#include "Core.h"

Core::Core() : receiver(PIN), eventHandler(&receiver, &knownSensorList, &codeMap)
{
    setupKnownSensors();
    receiverThread = thread(&Receiver::startReceiving, &receiver);
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
                ds->isEnabled((bool) stoi(field));
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
    if (it == knownSensorList.end()) {
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

void Core::fillBuffer(char *buffer, string s)
{
    for (int i = 0; i < s.length(); i++)
        buffer[i] = s[i];
}

bool Core::isFutureReady(shared_future<string> fut)
{
    return fut.wait_for(std::chrono::milliseconds(0)) == future_status::ready;
}

//TO TEST
void Core::registerNewDoorSensor(int clientSocket)
{
    abortProcedure = false;
    DoorSensor *ds = new DoorSensor();
    ds->setSensorID(getNewSensorID());
    ds->setSensorState(OPENED);

    std::shared_future<string> fut = std::async([this, clientSocket]() { return this->getMessageAndNotify(clientSocket); }).share();

    eventHandler.registerCode = true;
    unique_lock<mutex> registerLock(eventHandler.mNewCode);
    eventHandler.newCodeAvailable.wait(registerLock, [this, &fut] {
        return (bool)eventHandler.codeArrived || (bool)abortProcedure;
    });

    if (abortProcedure)
    {
        cout << "Abort...";
        eventHandler.registerCode = false;
        delete ds;
        return;

    }

    eventHandler.codeArrived = false;
    code closeCode = eventHandler.newCode;
    ds->setCloseCode(closeCode);
    cout << "Core - closeCode: " << closeCode << endl;

    sendMessage(clientSocket, Message::NEXT_CODE);

    bool go = true;
    while (go)
    {
        eventHandler.newCodeAvailable.wait(registerLock, [this, &fut] {
            return ((bool)eventHandler.codeArrived) || (bool) abortProcedure;
        });

        if (abortProcedure)
        {
            cout << "Abort...";
            eventHandler.registerCode = false;
            delete ds;
            return;
        }
        eventHandler.codeArrived = false;
        code openCode = eventHandler.newCode;
        if (openCode != closeCode)
        {
            ds->setOpenCode(openCode);
            sendMessage(clientSocket, MessageType::STRING);
            cout << "Core - openCode: " << openCode << endl;
            go = false;
        }
    }
    eventHandler.registerCode = false;
    
    if (!isFutureReady(fut)) {
        eventHandler.newCodeAvailable.wait(registerLock, [this, &fut] {
            return isFutureReady(fut);
        });
    }
    try{
        if (abortProcedure)
        {
            cout << "Abort...";
            delete ds;
            return;
        }
        else if (fut.get() == MessageType::STRING)
        {
            sendMessage(clientSocket, Message::ACK);
            string sensorName = getMessage(clientSocket);
            if (sensorName != fail && sensorName != Message::ABORT)
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
                delete ds;
        }
        else
            throw UnexpectedMessageException("Messaggio ricevuto dall'applicazione inaspettato");
    } catch(RegisterNewSensorException &e) {
        cout<<e.what()<<endl;
        sendMessage(clientSocket, Message::REGISTER_FAILED);
        delete ds;
        return;
    } 
    
    sendMessage(clientSocket, Message::REGISTER_SUCCESS);
    cout << "Core - FINE" << endl;
}

string Core::getMessage(int clientSocket)
{
    string message;
    char *buf = new char[BUFSIZ];
    string s = "";
    string app;
    bool go = true;
    bool header = true;
    do
    {
        int r = recv(clientSocket, buf, sizeof(buf), 0);
        if (r == 0 || r == SOCKET_ERROR)
        {
            message = fail;
            delete buf;
            eventHandler.newCodeAvailable.notify_all();
            return message;
        }
        message = message + ((string)buf).substr(0, r);
        if (message.find(eom) != string::npos)
        {
            message = message.substr(0, message.length() - eom.length());
            go = false;
        }
    } while (go);
    delete buf;
    return message;
}

string Core::getMessageAndNotify(int clientSocket) {
    string message = getMessage(clientSocket);
    abortProcedure = message == Message::ABORT || message == fail;
    eventHandler.newCodeAvailable.notify_all();
    return message;
}

void Core::sendMessage(int clientSocket, string message)
{
    char *buf = new char[BUFSIZ];
    fillBuffer(buf, message + eom);
    send(clientSocket, buf, message.length() + eom.length(), 0);
}



void Core::activateAlarm(int clientSocket) {
    if(isAlarmReady() && !eventHandler.alarmActivated) {
        eventHandler.alarmActivated = true;
        sendMessage(clientSocket, Message::ACTIVATION_SUCCESS);
    }
    else 
        sendMessage(clientSocket, Message::ACTIVATION_FAILED);
}

void Core::deactivateAlarm(int clientSocket) {
    if(eventHandler.alarmActivated) {
        eventHandler.alarmActivated = false;
        unique_lock<mutex> alarmLock(eventHandler.mAlarm);
        alarmLock.unlock();
        eventHandler.alarmDeactivated.notify_all();
        sendMessage(clientSocket, Message::DEACTIVATION_SUCCESS);
    }
    else {
        sendMessage(clientSocket, Message::DEACTIVATION_FAILED);
    }
}

void Core::sensorList(int clientSocket) {
    eventHandler.mSensorList.lock();
    for(Sensor* s : knownSensorList) 
        sendMessage(clientSocket, s->getSensorInfo());
    eventHandler.mSensorList.unlock();
    sendMessage(clientSocket, Message::END_SENSOR_LIST);
}
