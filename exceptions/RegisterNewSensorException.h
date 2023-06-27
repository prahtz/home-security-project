#include <iostream>
#include "Logger.h"
using namespace std;

class RegisterNewSensorException : runtime_error{
    public:

    RegisterNewSensorException(const char* what) : runtime_error(what) {}
    virtual const char* what() const noexcept override {
        return runtime_error::what();
    }
};

class UnexpectedMessageException : public RegisterNewSensorException{
    public:
    UnexpectedMessageException(const char* what) : RegisterNewSensorException(what) {
        Logger::log("EXCEPTION - Message unexpected during sensor registering");
    }
};

class TimeOutException : public RegisterNewSensorException{
    public:
    TimeOutException(const char* what) : RegisterNewSensorException(what) {
        Logger::log("EXCEPTION - Time expired during sensor registering");
    }
};


class AbortException : public RegisterNewSensorException{
    public:
    AbortException(const char* what) : RegisterNewSensorException(what) {
        Logger::log("EXCEPTION - Abort received during sensor registering");
    }
};


