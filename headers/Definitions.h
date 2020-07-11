#pragma once
#include <string>
#include <condition_variable>
#include <mutex>

typedef unsigned long code;
extern std::string SEPARATOR;
extern code activateSirenCode, deactivateSirenCode, ackActivateCode, ackDeactivateCode, tamperActiveCode, ackControlUnitCode, enableTamperCode, ackEnableTamperCode;

namespace statical {
    extern std::condition_variable sharedCondition;
    extern std::mutex mSharedCondition;
}