#pragma once

#include <string>
#include <list>
#include <fstream>
#include "Definitions.h"

using namespace std;

class FirebaseMessage {
    private:
        string authKey = "";
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