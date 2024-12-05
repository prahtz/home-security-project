#include "OAuth2.h"

string OAuth2::getJWT() {
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

string OAuth2::getToken() {
    time_t currentTime = time(nullptr);
    if (token == "" || currentTime - timeIssued > expiresIn) { 
        try {
            string jwt = getJWT();
            string httpBody = "grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion=" + jwt;
            string string_response = http_utils::sendPost(url, header, httpBody);
            if (json::accept(string_response)) {
                json response = json::parse(string_response);
                if (response.contains("access_token")) {
                    token = response["access_token"];
                    expiresIn = response["expires_in"];
                    timeIssued = time(nullptr);
                }
                else
                    throw OAuth2TokenNotReceivedException("OAuth2 request failed");
            }
        }
        catch (OAuth2TokenNotReceivedException e)
        {
            cout << e.what() << endl;
            return token;
        }
    }
    return token;
}
