#include "FirebaseNotification.h"

FirebaseNotification::FirebaseNotification() : FirebaseMessage(FIREBASE_NOTIFICATION_URL){
    std::ifstream f(NOTIFICATION_BODY_PATH);
    http_body = json::parse(f);
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
    http_body["message"]["data"]["n_type"] = n_type;
    http_body["message"]["token"] = token;
    http_body["message"]["android"]["ttl"] = ttl;
    return http_body.dump(4);
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