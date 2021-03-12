#include <exception>
#include <iostream>
using namespace std;

class RemoveSensorException : runtime_error{
    public:

    RemoveSensorException(const char* what) : runtime_error(what) {}
    virtual const char* what() const noexcept override {
        return runtime_error::what();
    }
};

class SensorNotFoundException : public RemoveSensorException{
    public:
    SensorNotFoundException(const char* what) : RemoveSensorException(what) {
        Logger::log("EXCEPTION - Sensor not found while removing sensor");
    }
};