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
        list<FirebaseNotification> notificationsBuffer;
    public:
        FirebaseMessagesHandler();
        void startService();
        void addNotification(FirebaseNotification notification);
};