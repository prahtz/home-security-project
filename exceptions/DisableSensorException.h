#include <exception>
#include <iostream>
using namespace std;

class DisableSensorException : runtime_error{
    public:

    DisableSensorException(const char* what) : runtime_error(what) {}
    virtual const char* what() const noexcept override {
        return runtime_error::what();
    }
};

class EnabledSensorNonFoundException : public DisableSensorException{
    public:
    EnabledSensorNonFoundException(const char* what) : DisableSensorException(what) {}
};

class SensorAlreadyDisabledException : public DisableSensorException{
    public:
    SensorAlreadyDisabledException(const char* what) : DisableSensorException(what) {}
};
