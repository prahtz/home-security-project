#pragma once
#include <string>
#include <fstream>
#include <filesystem>

#include "Definitions.h"
#include "TimeHandler.h"

using namespace std;

class Logger {
    private:
        static string wrapTimeToLog(date d, string s);
    public:
        static void log(string s);
};