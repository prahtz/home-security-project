#include <mutex>
#include <condition_variable>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include "FirebaseNotification.h"

using namespace curlpp::options;
using namespace std;

class FirebaseMessagesHandler {
    private:
        list<FirebaseMessage*> messagesBuffer;
    public:
        FirebaseMessagesHandler();
        void startService();
        void addMessage(FirebaseMessage* message);
};