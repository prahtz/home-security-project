#include "FirebaseMessage.h"

FirebaseMessage::FirebaseMessage(string url){
    addHeaderEntry("Content-Type:application/json");
    this->url = url;
}


list<string>& FirebaseMessage::getHeader() {
    return header;
}

string FirebaseMessage::getUrl() {
    return url;
}

void FirebaseMessage::setUrl(string url) {
    this->url = url;
}


void FirebaseMessage::addHeaderEntry(string entry) {
    header.push_back(entry);
}