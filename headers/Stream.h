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
    protected:
        list<T> eventList;
        list<Subscription<T>> subscriptionList;
        thread thisThread;
        atomic<bool> stop = false;
        atomic<bool> threadJoined = false;
        condition_variable newEvent;
        mutex mEventList;
        mutex mSubscriptionList;
    public:
        Stream();
        ~Stream();
        virtual void add(T event);
        virtual void startService();
        virtual void stopService();
        virtual Subscription<T>& listen(listener f);
        virtual Subscription<T>& listen();
};