#ifndef DISTRIBUTEDLOCK_TIMER_H
#define DISTRIBUTEDLOCK_TIMER_H
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>
#include <functional>
#include "common.h"


class Timer {
private:
    mutable std::mutex _mtx;
    mutable std::condition_variable _cv;
    bool _stop;
    std::function<void()> _func;
    milli_second _milli;
    std::shared_ptr<std::thread> _thread;
public:
    explicit Timer(std::function<void()> func, milli_second milli) : _stop(false), _func(std::move(func)), _milli(milli) {
        _thread = std::make_shared<std::thread>([this]() {
            while(true) {
                {
                    std::unique_lock<std::mutex> uq(_mtx);
                    if(_cv.wait_for(uq, _milli, [this]() { return _stop; })) break;
                }
                _func();
            }
        });
    }

    ~Timer() {
        {
            std::lock_guard<std::mutex> lg(_mtx);
            _stop = true;
        }
        _cv.notify_one();
        if(_thread->joinable()) _thread->join();
    }
};


#endif //DISTRIBUTEDLOCK_TIMER_H
