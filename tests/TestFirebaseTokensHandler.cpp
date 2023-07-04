#include "Core.h"
#include "PowerLoss.h"
#include <cassert>

class MockReceiver : public Receiver {
    public:
        void startReceiving() {
            code codeReceived = OPEN;
            codesBuffer.push_front(codeReceived);
            codeAvailable.notify_all();
        };
};

class MockTransmitter : public Transmitter {
    public:
        void startTransmittingProtocol() {};
        bool isCodeBufferEmpty() {
            return codesBuffer.empty();
        }
};

class MockFirebaseMessagesHandler : public FirebaseMessagesHandler {
    public:
        void startService() {};

        static bool isMessageBufferEmpty() {
            if (messagesBuffer.empty()) 
                return true;
            messagesBuffer = list<FirebaseMessage*>{};
            return false;
        }
};

class MockEventHandler : public EventHandler {
    public:
        MockEventHandler(
            MockReceiver* receiver, 
            MockTransmitter* transmitter, 
            MockFirebaseMessagesHandler* firebaseMessagesHandler)
        : EventHandler(receiver, transmitter, firebaseMessagesHandler)
            {}

        void activateDefenses() {
            EventHandler::activateDefenses();
        }
};

class MockPowerLoss : public PowerLoss {
    public:
        MockPowerLoss() {};
        void sendNotifications() {
            PowerLoss::sendNotifications();
        }
};

template <class T>
class MockStream : public Stream<T> {
    public:
        MockStream() : Stream<T>() {
            Stream<T>::stopService();
        }
        
        void add(T event) {
            Stream<T>::subscriptionList.back().getOnData()(event);
        }
        
        Subscription<T>& listen() override {
            Stream<T>::subscriptionList.emplace_back();
            Subscription<T>& sub = Stream<T>::subscriptionList.back();
            sub.stopService();
            return sub;
        }

        ~MockStream(){
        }

        void startService() {
            
        }
};

class MockTCPComm : public TCPComm {
    public:
        MockStream<string> m;

        MockTCPComm(int clientSocket) : TCPComm(clientSocket) {
            //TCPComm::stream.stopService();
        };

        void sendMessage(string message) {};

        Stream<string>& getMessageStream() override {
            Stream<string>& a = m;
            return a;
        }
};

class Test {
    private:
            MockReceiver receiver;
            MockTransmitter transmitter; 
            MockFirebaseMessagesHandler firebaseMessagesHandler; 
            MockEventHandler eventHandler = MockEventHandler(&receiver, &transmitter, &firebaseMessagesHandler);
            MockPowerLoss powerLoss;
    public:
        Test() {
            critical_section::firebaseTokensHandler->getTokenList().push_back("token1");
            critical_section::firebaseTokensHandler->getTokenList().push_back("token2");
            critical_section::firebaseTokensHandler->getTokenList().push_back("token3");
        }

        void testActivateDefenses() {
            assert(transmitter.isCodeBufferEmpty());
            assert(MockFirebaseMessagesHandler::isMessageBufferEmpty());
            eventHandler.activateDefenses();
            assert(!transmitter.isCodeBufferEmpty());
            assert(!MockFirebaseMessagesHandler::isMessageBufferEmpty());
        }

        void testPowerLossSendNotifications() {
            assert(MockFirebaseMessagesHandler::isMessageBufferEmpty());
            powerLoss.sendNotifications();
            assert(!MockFirebaseMessagesHandler::isMessageBufferEmpty());
        }

        void testCoreHandleFirebaseToken() {
            Core core;
            
            MockTCPComm tcpComm(0);
            TCPComm &t = tcpComm;
            core.handleFirebaseToken(t);

            Stream<string>& stream = tcpComm.getMessageStream();
            stream.add(message::STRING + "message");
            assert(critical_section::firebaseTokensHandler->getTokenList().back() == "message");
        }
};

int main () {
    Test t;
    t.testActivateDefenses();
    t.testPowerLossSendNotifications();
    t.testCoreHandleFirebaseToken();
}