#include <exception>
#include <iostream>
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
    UnexpectedMessageException(const char* what) : RegisterNewSensorException(what) {}
};

class TimeOutException : public RegisterNewSensorException{
    public:
    TimeOutException(const char* what) : RegisterNewSensorException(what) {}
};


class AbortException : public RegisterNewSensorException{
    public:
    AbortException(const char* what) : RegisterNewSensorException(what) {}
};


