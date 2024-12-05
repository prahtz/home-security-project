#pragma once

#include <string>
#include <list>
#include <fstream>

#include "Definitions.h"

using namespace std;

class FirebaseTokensHandler
{
private:
    list<string> tokenList;

public:
    void setupTokenList();
    void updateTokenList();
    list<string>& getTokenList();
};