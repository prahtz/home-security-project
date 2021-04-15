#pragma once
#include <string>
#include <list>
#include "Definitions.h"
#include "FirebaseMessage.h"

using namespace std;

class FirebaseOperation : public FirebaseMessage{
    private:
        string operationType;
        list<string> regIDs;
        const string projectId = "749962426974";
        const string notificationKeyName = "androidDevices";
        string notificationKey = "";

        string toString(list<string> regIDs);
    public:
        FirebaseOperation(string operationType);
        virtual string getHttpBody() override;
        string getProjectId();
        string getNotificationKeyName();
        string getNotificationKey();

        void addRegID(string regID);
};