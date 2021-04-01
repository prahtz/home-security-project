#include <iostream>
using namespace std;

class PinException : runtime_error{
    public:

    PinException(const char* what) : runtime_error(what) {}
    virtual const char* what() const noexcept override {
        return runtime_error::what();
    }
};


class PinTimeOutException : public PinException{
    public:
    PinTimeOutException(const char* what) : PinException(what) {
        Logger::log("EXCEPTION - Sensor not found while updating battery charge");
    }
};

class PinNotFoundException : public PinException{
    public:
    PinNotFoundException(const char* what) : PinException(what) {
        Logger::log("EXCEPTION - Sensor not found while updating battery charge");
    }
};