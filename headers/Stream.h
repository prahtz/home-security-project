#pragma once
#include <list>
#include <future>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <iterator>
#include <functional>
#include "Subscription.h"
#include <iostream>

using namespace std;


template <class T>
class Stream {
    typedef std::function<void(T)> listener;
    private:
        list<T> eventList;
        list<Subscription<T>*> subscriptionList;
        thread thisThread;
        atomic<bool> stopService = false;
        condition_variable newEvent;
        mutex mEventList;
        mutex mSubscriptionList;
    public:
        Stream();
        ~Stream();
        void add(T event);
        void startService();
        Subscription<T>* listen(listener f);
        Subscription<T>* listen();
};