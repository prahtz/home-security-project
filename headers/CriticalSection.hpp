#pragma once

#include <list>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <condition_variable>
#include "Locking.hpp"
#include "FirebaseTokensHandler.hpp"
#include "SensorsHandler.hpp"

using namespace std;

class critical_section {
    public:
        static inline Locking<FirebaseTokensHandler> firebaseTokensHandler = Locking<FirebaseTokensHandler>(FirebaseTokensHandler {});
        static inline Locking<SensorsHandler> sensorsHandler = Locking<SensorsHandler>(SensorsHandler {});
};