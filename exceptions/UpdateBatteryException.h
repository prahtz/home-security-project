#include <iostream>
using namespace std;

class UpdateBatteryException : runtime_error{
    public:

    UpdateBatteryException(const char* what) : runtime_error(what) {}
    virtual const char* what() const noexcept override {
        return runtime_error::what();
    }
};

class SensorChargedException : public UpdateBatteryException{
    public:
    SensorChargedException(const char* what) : UpdateBatteryException(what) {
        Logger::log("EXCEPTION - Sensor already charged");
    }
};

class UpdateBatterySensorNotFoundException : public UpdateBatteryException{
    public:
    UpdateBatterySensorNotFoundException(const char* what) : UpdateBatteryException(what) {
        Logger::log("EXCEPTION - Sensor not found while updating battery charge");
    }
};