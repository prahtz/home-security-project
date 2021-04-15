#include <iostream>
using namespace std;

class PinException : runtime_error{
    public:

    PinException(const char* what) : runtime_error(what) {}
    virtual const char* what() const noexcept override {
        return runtime_error::what();
    }
};


class PinNotFoundException : public PinException{
    public:
    PinNotFoundException(const char* what) : PinException(what) {
        Logger::log("EXCEPTION - PIN not found");
    }
};