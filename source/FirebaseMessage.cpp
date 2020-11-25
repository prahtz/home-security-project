#include "FirebaseMessage.h"

FirebaseMessage::FirebaseMessage(string url){
    header.push_back("Content-Type:application/json");
    header.push_back("Authorization:key=" + authKey);
    this->url = url;
}


list<string> FirebaseMessage::getHeader() {
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