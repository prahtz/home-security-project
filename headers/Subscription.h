#pragma once
#include <atomic>
#include <functional>
#include <future>
#include <mutex>
#include <list>
#include <condition_variable>
#include <iostream>
using namespace std;

template <typename T>
class Subscription {
    typedef std::function<void(T)> listener;
    protected:
        atomic<bool> canceled = false;
        atomic<bool> threadJoined = false;
        listener handler;
        future<void> execution;
        list<T> eventList;
        condition_variable newEvent;
        mutex mEventList;
        mutex mHandler;
    public:
        Subscription<T> (listener handler);
        Subscription<T> ();
        Subscription<T> (const Subscription<T>&){};
        ~Subscription<T>();
        virtual void startService();
        virtual void stopService();
        virtual void onData(listener handler);
        virtual void add(T event);
        virtual void cancel();
        virtual bool isCanceled();
        virtual listener getOnData();
};