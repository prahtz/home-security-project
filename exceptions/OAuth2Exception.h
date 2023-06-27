#include <iostream>
#include "Logger.h"
using namespace std;

class OAuth2Exception : runtime_error{
    public:

    OAuth2Exception(const char* what) : runtime_error(what) {}
    virtual const char* what() const noexcept override {
        return runtime_error::what();
    }
};


class OAuth2TokenNotReceivedException : public OAuth2Exception{
    public:
    OAuth2TokenNotReceivedException(const char* what) : OAuth2Exception(what) {
        Logger::log("EXCEPTION - OAuth2 token request failed");
    }
};