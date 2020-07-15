#pragma once
#include <string>
#include <condition_variable>
#include <mutex>

typedef unsigned long code;
extern std::string SEPARATOR;
static const code activateSirenCode = 14152368;
static const code deactivateSirenCode = 14476512;
static const code tamperActiveCode = 16557824;
static const code enableTamperCode = 10286572;

static const code ackActivateCode = 13354356;
static const code ackDeactivateCode = 13357324;
static const code ackControlUnitCode = 12343212;
static const code ackEnableTamperCode = 16752004;

namespace statical {
    extern std::condition_variable sharedCondition;
    extern std::mutex mSharedCondition;
}