#include <exception>
#include <iostream>
using namespace std;

class EnableSensorException : runtime_error{
    public:

    EnableSensorException(const char* what) : runtime_error(what) {}
    virtual const char* what() const noexcept override {
        return runtime_error::what();
    }
};

class DisabledSensorNotFoundException : public EnableSensorException{
    public:
    DisabledSensorNotFoundException(const char* what) : EnableSensorException(what) {}
};

class SensorAlreadyEnabledException : public EnableSensorException{
    public:
    SensorAlreadyEnabledException(const char* what) : EnableSensorException(what) {}
};

class SensorOpenedException : public EnableSensorException{
    public:
    SensorOpenedException(const char* what) : EnableSensorException(what) {}
};