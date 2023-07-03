#include "Stream.h"


template <class T>
Stream<T>::Stream() {
    thisThread = thread([this](){this->startService();});
}

template <class T>
void Stream<T>::startService() {
    while(!stop) {
        unique_lock<mutex> eventLock(mEventList);
        newEvent.wait(eventLock, [this] {
            return !eventList.empty() || stop;
        });
        if(eventList.empty())
            break;
        T event = eventList.front();
        eventList.pop_front();
        mSubscriptionList.lock();
        for (auto it = subscriptionList.begin(); it != subscriptionList.end(); ++it) {
            if(!(*it).isCanceled())
                (*it).add(event);
            else {
                it = subscriptionList.erase(it);
                it--;
            }
        }
        mSubscriptionList.unlock();
    }
}

template <class T>
void Stream<T>::stopService() {
    stop = true;
    newEvent.notify_all();
    if (!threadJoined) {
        thisThread.join();
        threadJoined = true;
    }
}

template <class T>
void Stream<T>::add(T event) {
    lock_guard<mutex> eventLock(mEventList);
    eventList.push_back(event);
    newEvent.notify_all();
}

template <class T>
Subscription<T>& Stream<T>::listen(listener f) {
    mSubscriptionList.lock();
    subscriptionList.emplace_back(f);
    Subscription<T> &s = subscriptionList.back();
    mSubscriptionList.unlock();
    return s;
}

template <class T> 
Subscription<T>& Stream<T>::listen() {
    mSubscriptionList.lock();
    subscriptionList.emplace_back();
    Subscription<T> &s = subscriptionList.back();
    mSubscriptionList.unlock();
    return s;
}

template <class T>
Stream<T>::~Stream() {
    stopService();
}

template class Stream<string>;
