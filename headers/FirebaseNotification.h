#pragma once
#include <string>
#include <list>
#include "Definitions.h"
#include "FirebaseMessage.h"

using namespace std;

class FirebaseNotification: public FirebaseMessage{
    private:
        string title = "";
        string body = "";
        string ttl = "120";
        string token = "APA91bFyPthniy3dbDzXVPAexa26XXSqcfL00xHY3_TNdoJELgADQ_Jw7BHieac7mIVY70duFaPhU-KkWHlTaWZ7ENmoAUevDxk-tw2oe0WdoOMG_-IPf6g";
    public:
        FirebaseNotification();
        string getBody();
        string getTitle();
        string getToken();
        string getTTL();
        virtual string getHttpBody() override;
        
        void setTitle(string title);
        void setBody(string body);
        void setToken(string token);
        void setTTL(string ttl);
};