#include "FirebaseNotification.h"

FirebaseNotification::FirebaseNotification(){
    header.push_back("Content-Type:application/json");
    header.push_back("Authorization:key=" + authKey);
}

string FirebaseNotification::getTitle() {
    return title;
}

string FirebaseNotification::getBody() {
    return body;
}

list<string> FirebaseNotification::getHeader() {
    return header;
}

string FirebaseNotification::getToken() {
    return token;
}

string FirebaseNotification::getUrl() {
    return url;
}

string FirebaseNotification::getHttpBody() {
    return "{ \"notification\": {\n\"title\": \"" + title + "\",\n\"body\": \"" + body +"\"\n},\n\"to\" : \"" + token + "\"\n}";
}

void FirebaseNotification::setUrl(string url) {
    this->url = url;
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

void FirebaseNotification::addHeaderEntry(string entry) {
    header.push_back(entry);
}