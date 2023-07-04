#pragma once

#include <iostream>
#include <mutex>
#include <list>
#include <functional>

template <class T>
class Locking
{
    T _data;
    mutable std::recursive_mutex _mtx;

    template <class U>
    class CallProxy
    {
        U *_p;
        std::recursive_mutex &_mtx;
        mutable bool _own;

    public:
        CallProxy(U *pp, std::recursive_mutex &mtx) : _p(pp), _mtx(mtx), _own(true)
        {
        }
        CallProxy(CallProxy const &other)
            : _p(other._p), _mtx(other._mtx), _own(true)
        {
            other._own = false;
        }
        ~CallProxy()
        {
            if (_own)
            {
                _mtx.unlock();
            }
        }

        CallProxy &operator=(CallProxy const &) = delete;

        U *operator->()
        {
            return _p;
        }
    };

public:
    template <class U>
    Locking(U &&data) : _data(std::forward<U>(data))
    {
    }

    CallProxy<T> operator->()
    {
        _mtx.lock();
        return CallProxy<T>(&_data, _mtx);
    }
    
    template <class R>
    R with_lock(std::function<R(T&)> f) {
        const std::lock_guard<std::recursive_mutex> lock(_mtx);
        return static_cast<R>(f(_data));
    }
};