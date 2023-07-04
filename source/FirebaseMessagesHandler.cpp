#include "FirebaseMessagesHandler.h" 

list<FirebaseMessage*> FirebaseMessagesHandler::messagesBuffer;

FirebaseMessagesHandler::FirebaseMessagesHandler() {

}

void FirebaseMessagesHandler::startService() {
    while(true) {
        unique_lock<mutex> lock(statical::mNewFirebaseNotification);
        statical::newFirebaseNotification.wait(lock, [this] () {return !messagesBuffer.empty();});
        FirebaseMessage* message = messagesBuffer.back();
        messagesBuffer.pop_back();
        string oauthToken = oauth2.getToken();
        message->addHeaderEntry("Authorization: Bearer " + oauthToken);
        
        json response = http_utils::sendPost(message->getUrl(), message->getHeader(), message->getHttpBody());
        if (response.contains("error")) {
            if (response["error"]["code"] == 404 || response["error"]["code"] == 400) {
                json body = json::parse(message->getHttpBody());
                string token = body["message"]["token"];
                critical_section::firebaseTokensHandler.with_lock<void>(
                    [token](FirebaseTokensHandler& firebaseTokensHandler) {
                        list<string>& tokenList = firebaseTokensHandler.getTokenList();
                        tokenList.remove(token);
                        firebaseTokensHandler.updateTokenList();
                    }
                );
            }
        }
        delete message;
    }
}

void FirebaseMessagesHandler::addMessage(FirebaseMessage* message) {
    messagesBuffer.push_front(message);
}