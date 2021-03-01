#pragma once
#include <string>
#include <condition_variable>
#include <mutex>

#define RECEIVER_PIN 25
#define TRANSMITTER_PIN 24

static const std::string PATH = "./csv";
static const std::string KNOWN_PATH = PATH + "/known.csv";
static const std::string TOKEN_PATH = PATH + "/tokens.csv";

typedef unsigned long code;
static const int BATTERY_LOW_SHIFT = 4;
static const std::string SEPARATOR = ";";
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

    extern std::condition_variable newFirebaseNotification;
    extern std::mutex mNewFirebaseNotification;
}