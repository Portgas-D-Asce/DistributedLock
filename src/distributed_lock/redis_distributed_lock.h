#ifndef REDISDISTRIBUTEDLOCK_REDIS_DISTRIBUTED_LOCK_H
#define REDISDISTRIBUTEDLOCK_REDIS_DISTRIBUTED_LOCK_H
#include <mutex>
#include <string>
#include <sstream>
#include <unistd.h>

#include <connection_pool/connection_pool.h>
#include <redis_pool/redis_connection.h>

#include "timer/timer.h"

class RedisDistributedLock {
private:
    std::string _key;
    // 自动过期时间（s）
    size_t _expire_time;
    // 过多久长时间续一次期（ms）
    size_t _renewal_time;
    // 加锁失败睡眠多久（ms）
    size_t _sleep_time;
    // 定时器
    std::shared_ptr<Timer> _timer;

    std::shared_ptr<spdlog::logger> _logger;
    const static char* const _lock_cmd;
    const static char* const _unlock_cmd;
    const static char* const _renewal_cmd;
public:
    explicit RedisDistributedLock(std::string key = "redis_distributed_key", size_t expire = 30,
                                  size_t renewal_time = 10000, size_t sleep_time = 50) : _key(std::move(key)),
                                  _expire_time(expire), _renewal_time(renewal_time), _sleep_time(sleep_time),
                                  _timer(nullptr), _logger(spdlog::stdout_color_mt("redis distributed key")) {
        _logger->set_level(spdlog::level::debug);
    }

    void lock();
    void unlock();
    // 超时加锁（s）
    bool try_lock(size_t timeout = 0);

private:
    // 必须可以独一无二地标记每个线程，且线程无论何时获取结果均一致
    static std::string uuid_fake();
    void renewal(const std::string& uuid);
};

#endif //REDISDISTRIBUTEDLOCK_REDIS_DISTRIBUTED_LOCK_H
