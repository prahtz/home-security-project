#include "FirebaseMessagesHandler.h" 

FirebaseMessagesHandler::FirebaseMessagesHandler() {

}

void FirebaseMessagesHandler::startService() {
    while(true) {
        unique_lock<mutex> lock(statical::mNewFirebaseNotification);
        statical::newFirebaseNotification.wait(lock, [this] () {return !messagesBuffer.empty();});
        FirebaseMessage* message = messagesBuffer.back();
        messagesBuffer.pop_back();
        try
        {
            curlpp::Cleanup myCleanup;
            curlpp::Easy myRequest;
           
            string httpBody = message->getHttpBody();
            myRequest.setOpt<Url>(message->getUrl());
            myRequest.setOpt<CaInfo>("./curl-ca-bundle.crt");
            myRequest.setOpt<HttpHeader>(message->getHeader());
            myRequest.setOpt<PostFields>(httpBody);
            myRequest.setOpt<PostFieldSize>(httpBody.length());

            myRequest.perform();
        }
        catch(curlpp::RuntimeError & e)
        {
            std::cout << e.what() << std::endl;
        }
        catch(curlpp::LogicError & e)
        {
            std::cout << e.what() << std::endl;
        }

    }
}

void FirebaseMessagesHandler::addMessage(FirebaseMessage* message) {
    messagesBuffer.push_front(message);
}