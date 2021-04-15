#include "Stream.h"


template <class T>
Stream<T>::Stream() {
    thisThread = thread([this](){this->startService();});
}

template <class T>
void Stream<T>::startService() {
    while(!stopService) {
        unique_lock<mutex> eventLock(mEventList);
        newEvent.wait(eventLock, [this] {
            return !eventList.empty() || stopService;
        });
        if(eventList.empty())
            break;
        T event = eventList.front();
        eventList.pop_front();
        mSubscriptionList.lock();
        for (auto it = subscriptionList.begin(); it != subscriptionList.end(); ++it) {
            if(!(*it)->isCanceled())
                (*it)->add(event);
            else {
                delete (*it);
                it = subscriptionList.erase(it);
                it--;
            }
        }
        mSubscriptionList.unlock();
    }
    for(Subscription<T>* s : subscriptionList)
        delete s;
}

template <class T>
void Stream<T>::add(T event) {
    lock_guard<mutex> eventLock(mEventList);
    eventList.push_back(event);
    newEvent.notify_all();
}

template <class T>
Subscription<T>* Stream<T>::listen(listener f) {
    Subscription<T>* s = new Subscription(f);
    mSubscriptionList.lock();
    subscriptionList.push_back(s);
    mSubscriptionList.unlock();
    return s;
}

template <class T>
Subscription<T>* Stream<T>::listen() {
    Subscription<T>* s = new Subscription<T>;
    subscriptionList.push_back(s);
    return s;
}

template <class T>
Stream<T>::~Stream() {
    stopService = true;
    newEvent.notify_all();
    thisThread.join();
}

template class Stream<string>;
