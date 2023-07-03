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

using namespace std;

class res {
    public:
        static inline Locking<FirebaseTokensHandler> firebaseTokensHandler = Locking<FirebaseTokensHandler>(FirebaseTokensHandler {});
};