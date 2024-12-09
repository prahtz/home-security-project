#pragma once

#include <list>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <condition_variable>
#include "Locking.hpp"
#include "FirebaseTokensHandler.h"
#include "SensorsHandler.h"
#include "ClientHandler.h"

class ClientHandler;

using namespace std;

class critical_section {
    public:
        static Locking<FirebaseTokensHandler> firebaseTokensHandler;
        static Locking<SensorsHandler> sensorsHandler;
        static Locking<ClientHandler> clientUpdater;
        static atomic<bool> alarmActivated, defensesActivated;
        static Locking<list<code>> codesBuffer;
        static condition_variable codeAvailable;
};