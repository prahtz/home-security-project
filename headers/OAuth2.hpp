#pragma once

#include <string>
#include <ctime>
#include <fstream>
#include <iostream>
#include <jwt/jwt_all.h>
#include "Definitions.h"
#include "HttpUtils.hpp"
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
        string getJWT() {
            std::ifstream f(SERVICE_ACCOUNT_PATH);
            json service_account = json::parse(f);
            std::time_t iat = std::time(nullptr);

            RS256Validator signer("", service_account["private_key"]);
            json payload = {
                {"iss", service_account["client_email"]},
                {"scope", "https://www.googleapis.com/auth/firebase.messaging"},
                {"aud", service_account["token_uri"]},
                {"exp", iat + tokenDuration}, 
                {"iat", iat}
            };

            return JWT::Encode(signer, payload);
        }

        string getToken() {
            time_t currentTime = time(nullptr);
            if (token == "" || currentTime - timeIssued > expiresIn) { 
                try {
                    string jwt = getJWT();
                    string httpBody = "grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion=" + jwt;
                    json response = http_utils::sendPost(url, header, httpBody);
                    if (response.contains("access_token")) {
                        token = response["access_token"];
                        expiresIn = response["expires_in"];
                        timeIssued = time(nullptr);
                    }
                    else
                        throw OAuth2TokenNotReceivedException("OAuth2 request failed");
                }
                catch (OAuth2TokenNotReceivedException e)
                {
                    cout << e.what() << endl;
                    return token;
                }
            }
            return token;
        }
};