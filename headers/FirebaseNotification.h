#pragma once
#include <string>
#include <list>
#include <fstream>
#include "Definitions.h"
#include "FirebaseMessage.h"
#include <nlohmann/json.hpp>

using namespace std;

using json = nlohmann::json;

class FirebaseNotification: public FirebaseMessage{
    private:
        string title = "";
        string body = "";
        string ttl = "120s";
        string n_type = "";
        string token = "";
        json http_body;
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