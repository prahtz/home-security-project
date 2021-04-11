#include "FirebaseOperation.h"
FirebaseOperation::FirebaseOperation(string operationType) : FirebaseMessage("https://fcm.googleapis.com/fcm/notification") {
    this->operationType = operationType;
    addHeaderEntry("project_id:" + projectId);
    ifstream in(ANDROID_GROUP_KEY_PATH);
    in >> notificationKey;
    in.close();
}

string FirebaseOperation::getProjectId() {
    return projectId;
}

string FirebaseOperation::getNotificationKey() {
    return notificationKey;
}

string FirebaseOperation::getNotificationKeyName() {
    return notificationKeyName;
}

string FirebaseOperation::getHttpBody() {
    return "{ \"operation\": \"" + operationType + "\",\n\"notification_key_name\": \"" 
        + notificationKeyName +"\",\n\"notification_key\": \"" + notificationKey + "\",\n\"registration_ids\": [" + toString(regIDs) + "]\n}";
}

void FirebaseOperation::addRegID(string regID) {
    this->regIDs.push_back(regID);
}

string FirebaseOperation::toString(list<string> regIDs) {
    string result = "";
    for(string s : regIDs) {
        result = result + "\"" + s + "\", ";
    }
    result.substr(0, result.length() - 2);
    return result;
}