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
        string n_type = "";
        string token = "";
    public:
        FirebaseNotification();
        string getBody();
        string getTitle();
        string getToken();
        string getTTL();
        string getNType();
        virtual string getHttpBody() override;
        
        void setTitle(string title);
        void setBody(string body);
        void setToken(string token);
        void setTTL(string ttl);
        void setNType(string n_type);
};