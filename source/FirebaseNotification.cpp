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
    return "{ \"priority\": \"high\",\n\"data\": {\n\"n_type\": \"" + n_type + "\"\n\"click_action\": \"FLUTTER_NOTIFICATION_CLICK\"},  \"android\": { \"time_to_live\": \"" + ttl + "\" },\n\"to\" : \"" + token + "\"\n}";
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

void FirebaseNotification::setNType(string n_type) {
    this->n_type = n_type;
}