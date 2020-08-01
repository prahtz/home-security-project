#include "FirebaseMessagesHandler.h" 

FirebaseMessagesHandler::FirebaseMessagesHandler() {

}

void FirebaseMessagesHandler::startService() {
    while(true) {
        unique_lock<mutex> lock(statical::mNewFirebaseNotification);
        statical::newFirebaseNotification.wait(lock, [this] () {return !notificationsBuffer.empty();});
        FirebaseNotification notification = notificationsBuffer.back();
        notificationsBuffer.pop_back();
        try
        {
            curlpp::Cleanup myCleanup;
            curlpp::Easy myRequest;
           
            string httpBody = notification.getHttpBody();
            myRequest.setOpt<Url>(notification.getUrl());
            myRequest.setOpt<CaInfo>("./curl-ca-bundle.crt");
            myRequest.setOpt<HttpHeader>(notification.getHeader());
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

void FirebaseMessagesHandler::addNotification(FirebaseNotification notification) {
    notificationsBuffer.push_front(notification);
}