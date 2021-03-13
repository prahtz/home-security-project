#include "Logger.h"

void Logger::log(string s) {
    date d = TimeHandler::getDate();
    string fileDir = LOG_PATH + "/" + to_string(d.yy) + (d.mm < 10 ? "/0" : "/") + to_string(d.mm);
    std::filesystem::create_directories(fileDir);
    string fileName = (d.dd < 10 ? "0" : "") + to_string(d.dd) + ".log";
    ofstream out(fileDir + "/" + fileName, ios::app);
    s = wrapTimeToLog(d, s);
    out << s << endl;
    out.close();
} 

string Logger::wrapTimeToLog(date d, string s) {
    return to_string(d.hh) + ":" + to_string(d.min) + ":" + to_string(d.sec) + " -> " + s;
}