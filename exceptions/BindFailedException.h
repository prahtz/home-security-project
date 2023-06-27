#include <iostream>
#include "Logger.h"
using namespace std;

class BindFailedException : runtime_error{
    public:

    BindFailedException(const char* what) : runtime_error(what) {}
    virtual const char* what() const noexcept override {
        return runtime_error::what();
    }
};