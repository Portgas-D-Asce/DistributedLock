#ifndef DISTRIBUTEDLOCK_TIMER_H
#define DISTRIBUTEDLOCK_TIMER_H
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>
#include <functional>


class Timer {
private:
    mutable std::mutex _mtx;
    mutable std::condition_variable _cv;
    bool _stop;
    std::function<void()> _func;
    size_t _milli;
    std::shared_ptr<std::thread> _thread;
public:
    explicit Timer(std::function<void()> func,  size_t milli)
        : _stop(false), _func(std::move(func)), _milli(milli) {
        _thread = std::make_shared<std::thread>([this]() { routing(); });
    }

    ~Timer() {
        {
            std::lock_guard<std::mutex> lg(_mtx);
            _stop = true;
        }
        _cv.notify_one();
        if(_thread->joinable()) _thread->join();
    }
private:
    void routing() {
        while(true) {
            std::unique_lock<std::mutex> uq(_mtx);
            if(_cv.wait_for(uq, static_cast<std::chrono::duration<size_t, std::milli>>(_milli),
                            [this]() { return _stop; })) break;

            _func();
        }

    }

};


#endif //DISTRIBUTEDLOCK_TIMER_H
