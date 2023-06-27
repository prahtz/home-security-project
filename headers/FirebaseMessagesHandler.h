#pragma once
#include <mutex>
#include <condition_variable>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include "FirebaseMessage.h"
#include "OAuth2.hpp"

using namespace curlpp::options;
using namespace std;

class FirebaseMessagesHandler {
    private:
        static list<FirebaseMessage*> messagesBuffer;
        OAuth2 oauth2;
    public:
        
        FirebaseMessagesHandler();
        void startService();
        static void addMessage(FirebaseMessage* message);
};