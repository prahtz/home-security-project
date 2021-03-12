#include "TimeHandler.h"

date TimeHandler::getDate() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&tt);
    return {local_tm.tm_year + 1900, local_tm.tm_mon + 1, 
        local_tm.tm_mday, local_tm.tm_hour, 
        local_tm.tm_min, local_tm.tm_sec};
}

void TimeHandler::sleepUntillNewDay() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm *local_tm = localtime(&tt);
    incrementDate(local_tm);
    local_tm->tm_sec = 0;
    local_tm->tm_min = 0;
    local_tm->tm_hour = 0;
    std::this_thread::sleep_until (std::chrono::system_clock::from_time_t (mktime(local_tm)));
}

void TimeHandler::incrementDate(tm* tm) {
    int dd = tm->tm_mday;
    int mm = tm->tm_mon + 1;
    int yy = tm->tm_year + 1900;
    if(isDayIncrementable(dd, mm , yy))
        ++tm->tm_mday;
    else {
        tm->tm_mday = 1;
        if(mm == 12) {
            tm->tm_mon = 1;
            ++tm->tm_year;
        }
        else 
            ++tm->tm_mon;
    }
}

bool TimeHandler::isLeapYear(int yy) {
    if(yy % 400 == 0)
        return true;
    else if(yy % 4 == 0)
        return true;
    return false;
}

bool TimeHandler::isDayIncrementable(int dd, int mm, int yy) {
    return (dd <= 29 && mm != 2) || (dd == 28 && mm == 2 && isLeapYear(yy)) || (dd == 30 && !is30dayMonth(mm));
}

bool TimeHandler::is30dayMonth(int mm) {
    return mm == 4 || mm == 6 || mm == 9 || mm == 11;
}

