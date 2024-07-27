#ifndef DISTRIBUTEDLOCK_TIMER_H
#define DISTRIBUTEDLOCK_TIMER_H
#include <mutex>
#include <thread>
#include <memory>
#include <chrono>
#include <condition_variable>
#include <functional>

using day = std::chrono::duration<size_t, std::ratio<24 * 60 * 60>>;
using hour = std::chrono::duration<size_t, std::ratio<60 * 60>>;
using minute = std::chrono::duration<size_t, std::ratio<60>>;
using second = std::chrono::duration<size_t>;
using milli_second = std::chrono::duration<size_t, std::milli>;
using micro_second = std::chrono::duration<size_t, std::micro>;
using nano_second = std::chrono::duration<size_t, std::nano>;

class Timer {
private:
    mutable std::mutex _mtx;
    mutable std::condition_variable _cv;
    bool _stop;
    std::function<void()> _func;
    milli_second _milli;
    std::shared_ptr<std::thread> _thread;
public:
    explicit Timer(std::function<void()> func, size_t milli)
        : _stop(false),_func(std::move(func)), _milli(static_cast<milli_second>(milli)) {

        // 定时器运行期间，条件变量永远为假，等待超时时间后，执行一次 func
        // 一旦被唤醒且条件变量为真，说明定时任务已被取消，直接结束
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
        // 停止定时任务
        {
            std::lock_guard<std::mutex> lg(_mtx);
            _stop = true;
        }
        // 通知定时任务线程
        _cv.notify_one();
        // 等待定时任务线程结束
        if(_thread->joinable()) _thread->join();
    }
};


#endif //DISTRIBUTEDLOCK_TIMER_H
