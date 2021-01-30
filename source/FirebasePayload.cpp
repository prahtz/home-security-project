#include "FirebasePayload.h"

FirebasePayload::FirebasePayload() : FirebaseMessage("https://fcm.googleapis.com/fcm/send"){

}

string FirebasePayload::getTitle() {
    return title;
}

string FirebasePayload::getBody() {
    return body;
}

string FirebasePayload::getHttpBody() {
    //return "{ \"notification\": {\n\"title\": \"" + title + "\",\n\"body\": \"" + body +"\"\n},\n\"to\" : \"" + token + "\"\n}";
    return "{ \"priority\": \"high\",\n\"data\": {\n\"click_action\": \n \"FLUTTER_NOTIFICATION_CLICK\"},  \"android\": { \"ttl\": \"" + ttl + "\" },\n\"to\" : \"" + token + "\"\n}";
}

void FirebasePayload::setTitle(string title) {
    this->title = title;
}
        
void FirebasePayload::setBody(string body) {
    this->body = body;
}

void FirebasePayload::setToken(string token) {
    this->token = token;
}