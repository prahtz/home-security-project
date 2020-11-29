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
        const string notificationKey = "APA91bFyPthniy3dbDzXVPAexa26XXSqcfL00xHY3_TNdoJELgADQ_Jw7BHieac7mIVY70duFaPhU-KkWHlTaWZ7ENmoAUevDxk-tw2oe0WdoOMG_-IPf6g";

        string toString(list<string> regIDs);
    public:
        FirebaseOperation(string operationType);
        virtual string getHttpBody() override;
        string getProjectId();
        string getNotificationKeyName();
        string getNotificationKey();

        void addRegID(string regID);
};