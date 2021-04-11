#include "Subscription.h"

template <typename T>
Subscription<T>::Subscription(listener handler)
{
    this->handler = handler;
    this->execution = std::async(std::launch::async, [this]{startService();});
}

template <typename T>
Subscription<T>::Subscription()
{
    this->handler = [](T n) {};
    this->execution = std::async(std::launch::async, [this]{startService();});
}

template <typename T>
void Subscription<T>::startService()
{
    while (!canceled.load())
    {
        unique_lock<mutex> eventLock(mEventList);
        newEvent.wait(eventLock, [this] { 
            return !eventList.empty() || canceled;
        });
        if(eventList.empty())
            break;
        T event = eventList.front();
        eventList.pop_front();
        eventLock.unlock();
        mHandler.lock();
        listener h = handler;
        mHandler.unlock();
        h(event);
    }
}

template<typename T>
void Subscription<T>::add(T event) {
    lock_guard<mutex> eventLock(mEventList);
    eventList.push_back(event);
    newEvent.notify_all();
}

template <typename T>
void Subscription<T>::onData(listener handler)
{
    lock_guard<mutex> handlerLock(mHandler);
    this->handler = handler;
}


template <typename T>
typename Subscription<T>::listener Subscription<T>::getOnData()
{
    return handler;
}

template <typename T>
void Subscription<T>::cancel()
{
    canceled = true;
    newEvent.notify_all();
}

template <typename T>
bool Subscription<T>::isCanceled()
{
    return canceled.load();
}

template <typename T>
Subscription<T>::~Subscription()
{
    this->cancel();
    this->execution.wait();
}

template class Subscription<string>;