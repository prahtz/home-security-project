#pragma once
#include <atomic>
#include <functional>
#include <future>
#include <mutex>
#include <list>
#include <condition_variable>
using namespace std;


template <typename T>
class Subscription {
    typedef std::function<void(T)> listener;
    private:
        atomic<bool> canceled = false;
        listener handler;
        future<void> execution;
        list<T> eventList;
        condition_variable newEvent;
        mutex mEventList;
        mutex mHandler;
        void startService();
    public:
        Subscription<T> (listener handler);
        Subscription<T> ();
        ~Subscription<T>();
        void onData(listener handler);
        void add(T event);
        void cancel();
        bool isCanceled();
        listener getOnData();
};