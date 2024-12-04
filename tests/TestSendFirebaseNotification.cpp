#include "Core.h"

class MockFirebaseMessagesHandler : public FirebaseMessagesHandler {
    public:
        void startService(string token) {
            FirebaseNotification* notification = new FirebaseNotification();
            notification->setTitle("ALLARME ATTIVO");
            notification->setBody("Intrusione rilevata!");
            notification->setToken(token);
            string oauthToken = oauth2.getToken();
            notification->addHeaderEntry("Authorization: Bearer " + oauthToken);
            string string_response = http_utils::sendPost(notification->getUrl(), notification->getHeader(), notification->getHttpBody());
        };
};

class Test {
    private:
        MockFirebaseMessagesHandler firebaseMessagesHandler;
    public:
        Test() {
            
        }

        void testSendNotification(string token) {
            firebaseMessagesHandler.startService(token);
        }
};

int main (int argc, char *argv[]) {
    Test t;
    if (argc != 2) {
        cout << "Invalid arguments" << endl;
		return -1;
    }
    t.testSendNotification(argv[1]);
}