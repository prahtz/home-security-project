#pragma once

#include <string>
#include <list>
#include "Definitions.h"

using namespace std;

class FirebaseMessage {
    private:
        string authKey = "AAAArp0-el4:APA91bFPiEP_Q0o8J6jP_9EvuUpFjcEqe-IG9XQF62WCiqJO9NJH7F_Qusdv2vGf1qiyvAIm5LqWUOkZvZLXpGrar1x34rCvzOvnyp6IUvr4H_mXqfCHoNoHyKP3BbRqwGTu4Q1HSOmR";
        string url;

        list<string> header;
    public:
        FirebaseMessage(string url);
        string getUrl();
        list<string> getHeader();
        virtual string getHttpBody() = 0;
        
        void setUrl(string url);
        void addHeaderEntry(string entry);
};