#pragma once

#include <string>
#include <ctime>
#include <fstream>
#include <iostream>
#include <jwt/jwt_all.h>
#include "Definitions.h"
#include "HttpUtils.h"
#include "Exceptions.h"

using namespace std;
using json = nlohmann::json;

class OAuth2{
    private:
        string token = "";
        time_t timeIssued = -1;
        long tokenDuration = JWT_TOKEN_DURATION;
        long expiresIn = 0;

        string url = "https://oauth2.googleapis.com/token";
        list<string> header = {
            "Host: oauth2.googleapis.com", 
            "Content-Type: application/x-www-form-urlencoded"
        };
    public:
        string getJWT();
        string getToken();
};