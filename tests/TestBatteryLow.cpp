#include "Core.h"
#include <cassert>

class MockReceiver : public Receiver
{
public:
    void startReceiving()
    {
        code codeReceived = min(9704202, 9704206) - BATTERY_LOW_SHIFT;
        codesBuffer.push_front(codeReceived);
        codeAvailable.notify_all();
    };
};

class MockTransmitter : public Transmitter
{
public:
    void startTransmittingProtocol() {};
    bool isCodeBufferEmpty()
    {
        return codesBuffer.empty();
    }
};

class MockFirebaseMessagesHandler : public FirebaseMessagesHandler
{
public:
    void startService() {};

    static bool isMessageBufferEmpty()
    {
        if (messagesBuffer.empty())
            return true;
        messagesBuffer = list<FirebaseMessage *>{};
        return false;
    }
};

class MockEventHandler : public EventHandler
{
public:
    MockEventHandler(
        MockReceiver *receiver,
        MockTransmitter *transmitter,
        MockFirebaseMessagesHandler *firebaseMessagesHandler)
        : EventHandler(receiver, transmitter, firebaseMessagesHandler)
    {
    }

    void startListening()
    {
    }

    void testBatteryLow()
    {
        unique_lock<mutex> receiverLock(receiver->mBuff);
        receiver->codeAvailable.wait(receiverLock, [this]
                                     { return !receiver->isBufferEmpty(); });

        code codeReceived = receiver->popCodeFromBuffer();
        critical_section::sensorsHandler.with_lock<void>([this, codeReceived](SensorsHandler &sensorsHandler)
                                                         {
            sensorsHandler.setupKnownSensors();
            map<code, pair<Action, Sensor *> *>& codeMap = sensorsHandler.getCodeMap();
            map<code, pair<Action, Sensor *> *>::iterator mapIterator = codeMap.find(codeReceived);
            bool sensorCode = codeMap.end() != mapIterator;
            cout << sensorCode << endl;
            if (sensorCode)
            {
                Action action = mapIterator->second->first;
                Sensor *sensor = mapIterator->second->second;
                switch (action)
                {
                    case BATTERY_LOW:
                        cout << "found" << endl;
                }
            } 
        });
    }
};

class Test
{
private:
    MockReceiver receiver;
    MockTransmitter transmitter;
    MockFirebaseMessagesHandler firebaseMessagesHandler;
    MockEventHandler eventHandler = MockEventHandler(&receiver, &transmitter, &firebaseMessagesHandler);

public:
    Test()
    {
    }

    void testOnBatteryLow()
    {
        receiver.startReceiving();
        eventHandler.testBatteryLow();
    }
};

int main()
{
    Test t;
    t.testOnBatteryLow();
}