#include "FirebaseNotification.h"

FirebaseNotification::FirebaseNotification() : FirebaseMessage("https://fcm.googleapis.com/fcm/send"){

}

string FirebaseNotification::getTitle() {
    return title;
}

string FirebaseNotification::getBody() {
    return body;
}

string FirebaseNotification::getTTL() {
    return ttl;
}

string FirebaseNotification::getHttpBody() {
    //return "{ \"notification\": {\n\"title\": \"" + title + "\",\n\"body\": \"" + body +"\"\n},\n\"to\" : \"" + token + "\"\n}";
    return "{ \"priority\": \"high\",\n\"data\": {\n\"click_action\": \n \"FLUTTER_NOTIFICATION_CLICK\"},  \"android\": { \"time_to_live\": \"" + ttl + "\" },\n\"to\" : \"" + token + "\"\n}";
}

void FirebaseNotification::setTitle(string title) {
    this->title = title;
}
        
void FirebaseNotification::setBody(string body) {
    this->body = body;
}

void FirebaseNotification::setToken(string token) {
    this->token = token;
}

void FirebaseNotification::setTTL(string ttl) {
    this->ttl = ttl;
}