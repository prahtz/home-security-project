#pragma once
#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

struct date {
    int yy;
    int mm;
    int dd;
    int hh;
    int min;
    int sec;
};



class TimeHandler {
    private:
        static void incrementDate(tm* tm);
        static bool isLeapYear(int yy);
        static bool is30dayMonth(int mm);
        static bool isDayIncrementable(int dd, int mm, int yy);
    public:
        inline static std::chrono::time_point<std::chrono::steady_clock> start;
        static date getDate();
        static void sleepUntillNewDay();
        static void startTimer();
        static int elapsedSeconds();
};