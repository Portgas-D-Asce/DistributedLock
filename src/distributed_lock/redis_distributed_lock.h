#ifndef REDISDISTRIBUTEDLOCK_REDIS_DISTRIBUTED_LOCK_H
#define REDISDISTRIBUTEDLOCK_REDIS_DISTRIBUTED_LOCK_H
#include <mutex>
#include <string>
#include <chrono>
#include <sstream>
#include <unistd.h>
#include <hiredis/hiredis.h>
#include "singleton.h"
#include "connection_pool.h"
#include "timer.h"

class RedisDistributedLock {
private:
    std::string _key;
    size_t _expire;
    std::shared_ptr<Timer> _timer;
public:
    explicit RedisDistributedLock(std::string key = "redis_distributed_key", size_t expire = 30)
        : _key(std::move(key)), _expire(expire), _timer(nullptr) {}

    void lock();

    bool try_lock(std::chrono::duration<size_t> timeout = std::chrono::duration<size_t>(0));

    void unlock();
private:
    // 必须可以独一无二地标记每个线程，且线程无论何时获取结果均一致
    static std::string uuid_fake();

    void renewal(const std::string& uuid);
};

void RedisDistributedLock::lock() {
    try_lock(std::chrono::duration<size_t>(2147483647));
}

bool RedisDistributedLock::try_lock(std::chrono::duration<size_t> timeout) {
    auto conn = Singleton<RedisPool>::get_instance().get();
    assert(conn && "get connection failed when locking redis distributed lock!");
    auto c = conn->ctx();
    std::string cmd = "if redis.call('exists', KEYS[1]) == 0 or redis.call('hexists', KEYS[1], ARGV[1]) == 1 then "
                      "redis.call('hincrby', KEYS[1], ARGV[1], 1) "
                      "redis.call('expire', KEYS[1], ARGV[2]) "
                      "return 1 "
                      "else "
                      "return 0 "
                      "end";

    auto try_once = [this, &c, &cmd]() {
        redisReply* reply = static_cast<redisReply *>(redisCommand(c,"eval %s 1 %s %s %s",
            cmd.c_str(),_key.c_str(), uuid_fake().c_str(), std::to_string(_expire).c_str()));

        size_t res = reply->integer;
        freeReplyObject(reply);
        return res;
    };

    // 条件变量的 wait_until or 休眠一段时间？？？？？？？
    // 条件变量比较尴尬，notify 时只有本进程的线程可以收到通知（其实，这样也还行？？？？？）
    // 至少需要尝试一次，否则当 timeout 是 0 时，永远不会成功。
    std::chrono::steady_clock::time_point abs =  std::chrono::steady_clock::now() + timeout;
    do {
        if(try_once()) {
            std::string uuid = uuid_fake();
            if(!_timer) _timer = std::make_shared<Timer>([this, uuid]() { renewal(uuid); }, _expire * 1000 / 3);
            return true;
        }
        std::this_thread::sleep_for(std::chrono::duration<size_t, std::milli>(20));
    } while(std::chrono::steady_clock::now() <= abs);

    return false;
}

void RedisDistributedLock::unlock() {
    auto conn = Singleton<RedisPool>::get_instance().get();
    assert(conn && "get connection failed when unlocking redis distributed lock!");
    auto c = conn->ctx();

    std::string cmd = "if redis.call('hexists', KEYS[1], ARGV[1]) == 0 then "
                      "return nil "
                      "elseif redis.call('hincrby', KEYS[1], ARGV[1], -1) == 0 then "
                      "return redis.call('del', KEYS[1]) "
                      "else "
                      "return 0 "
                      "end";
    redisReply* reply = static_cast<redisReply *>(
        redisCommand(c,"eval %s 1 %s %s", cmd.c_str(), _key.c_str(), uuid_fake().c_str()));

    assert(reply->type != 4 && "distributed lock was deleted in a wrong way\n");
    if(reply->integer == 1) _timer = nullptr;
    freeReplyObject(reply);
}

void RedisDistributedLock::renewal(const std::string& uuid) {
    auto conn = Singleton<RedisPool>::get_instance().get();
    assert(conn && "get connection failed when unlocking redis distributed lock!");
    auto c = conn->ctx();

    std::string cmd = "if redis.call('hexists', KEYS[1], ARGV[1]) == 1 then "
                          "return redis.call('expire', KEYS[1], ARGV[2]) "
                      "else "
                          "return 0 "
                      "end";

    redisReply* reply = static_cast<redisReply *>(
        redisCommand(c,"eval %s 1 %s %s %s", cmd.c_str(), _key.c_str(), uuid.c_str(),
                     std::to_string(_expire).c_str()));

    assert(reply->integer == 1 && "renewal distributed lock failed\n");
    freeReplyObject(reply);
}

std::string RedisDistributedLock::uuid_fake() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string tid = ss.str();
    tid += std::to_string(getpid());
    return tid;
}


#endif //REDISDISTRIBUTEDLOCK_REDIS_DISTRIBUTED_LOCK_H
