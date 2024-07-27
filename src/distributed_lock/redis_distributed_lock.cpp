#include "redis_distributed_lock.h"


const char* const RedisDistributedLock::_lock_cmd =
    "if redis.call('exists', KEYS[1]) == 0 or redis.call('hexists', KEYS[1], ARGV[1]) == 1 then "
    "redis.call('hincrby', KEYS[1], ARGV[1], 1) "
    "redis.call('expire', KEYS[1], ARGV[2]) "
    "return 1 "
    "else "
    "return 0 "
    "end";

const char* const RedisDistributedLock::_unlock_cmd =
    "if redis.call('hexists', KEYS[1], ARGV[1]) == 0 then "
    "return nil "
    "elseif redis.call('hincrby', KEYS[1], ARGV[1], -1) == 0 then "
    "return redis.call('del', KEYS[1]) "
    "else "
    "return 0 "
    "end";

const char* const RedisDistributedLock::_renewal_cmd =
    "if redis.call('hexists', KEYS[1], ARGV[1]) == 1 then "
    "return redis.call('expire', KEYS[1], ARGV[2]) "
    "else "
    "return 0 "
    "end";

void RedisDistributedLock::lock() {
    try_lock(INT_MAX);
}

bool RedisDistributedLock::try_lock(size_t timeout) {
    auto conn = SingleConnectionPool<RedisConnection>::get_instance().get();
    assert(conn && "get connection failed when locking redis distributed lock!");

    auto try_once = [this, &conn]() {
        redisReply* reply = static_cast<redisReply *>(
            redisCommand(conn->ctx(),"eval %s 1 %s %s %s", _lock_cmd, _key.c_str(),
                         uuid_fake().c_str(), std::to_string(_expire_time).c_str()));

        size_t res = reply->integer;
        freeReplyObject(reply);
        return res;
    };

    // 至少需要尝试一次，否则当 timeout 是 0 时，永远不会成功
    auto abs =  std::chrono::steady_clock::now() + std::chrono::duration<size_t>(timeout);
    std::string uuid = uuid_fake();
    do {
        if(try_once()) {
            // 加锁成功
            if(!_timer) {
                // 创建分布式锁时，立刻启动续期定时器
                _logger->debug("create redis distributed lock {} and start timer", uuid_fake());
                _timer = std::make_shared<Timer>([this, uuid]() { renewal(uuid); }, _renewal_time);
            } else {
                // 递归加锁
                _logger->debug("incr redis distributed lock {}", uuid_fake());
            }
            return true;
        } else {
            // 加锁，失败休眠一段时间（条件变量比较尴尬，notify 只会通知本进程的线程，会导致进程独占分布式锁）
            std::this_thread::sleep_for(milli_second(_sleep_time));
        }
    } while(std::chrono::steady_clock::now() <= abs);

    return false;
}

void RedisDistributedLock::unlock() {
    auto conn = SingleConnectionPool<RedisConnection>::get_instance().get();
    assert(conn && "get connection failed when unlocking redis distributed lock!");

    redisReply* reply = static_cast<redisReply *>(
        redisCommand(conn->ctx(),"eval %s 1 %s %s", _unlock_cmd, _key.c_str(), uuid_fake().c_str()));

    // 返回值为 nil，分布式锁不存在了，可能是被删除了，也可能是没续上期，都属于异常情况
    assert(reply->type != 4 && "distributed lock was deleted in a wrong way\n");

    if(reply->integer == 1) {
        // 引用计数为 0，释放分布式锁，终止定时续期线程
        _logger->debug("free distributed lock {} and stop timer", uuid_fake());
        _timer = nullptr;
    } else {
        // 减少锁的引用计数，不终止定时器
        _logger->debug("decr distributed lock {}", uuid_fake());
    }
    freeReplyObject(reply);
}

void RedisDistributedLock::renewal(const std::string& uuid) {
    auto conn = SingleConnectionPool<RedisConnection>::get_instance().get();
    assert(conn && "get connection failed when unlocking redis distributed lock!");

    redisReply* reply = static_cast<redisReply *>(
        redisCommand(conn->ctx(),"eval %s 1 %s %s %s", _renewal_cmd, _key.c_str(),
                     uuid.c_str(), std::to_string(_expire_time).c_str()));

    // 续期失败
    assert(reply->integer == 1 && "renewal distributed lock failed\n");
    _logger->debug("renewal succeeded {}", uuid_fake());
    freeReplyObject(reply);
}

std::string RedisDistributedLock::uuid_fake() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string tid = ss.str();
    tid += std::to_string(getpid());
    return tid;
}
