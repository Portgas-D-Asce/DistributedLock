#ifndef CONNECTIONPOOL_REDIS_CONNECTION_H
#define CONNECTIONPOOL_REDIS_CONNECTION_H
#include <hiredis/hiredis.h>
#include "redis_connection_option.h"
#include "connection.h"


class RedisConnection : public Connection {
public:
    using ConnectionOption = RedisConnectionOption;
    using ConnectionOptionPtr = std::shared_ptr<ConnectionOption>;
    using ConnectionPtr = std::shared_ptr<RedisConnection>;
private:
    ConnectionOptionPtr _opt;
    redisContext* _ctx;
public:
    explicit RedisConnection(ConnectionOptionPtr opt) : _opt(std::move(opt)), _ctx(nullptr) {}

    ~RedisConnection() override { if(_ctx) redisFree(_ctx); }

    auto ctx() const { return _ctx; }

    bool connect(const struct timeval& timeout) override;
};

bool RedisConnection::connect(const struct timeval& timeout) {
    _ctx = redisConnectWithTimeout(_opt->host().c_str(), _opt->port(), timeout);

    if(_ctx && !_ctx->err) {
        printf("connection success!\n");
        return true;
    }
    printf("connection failed!\n");
    if(_ctx) {
        printf("connect error: %s\n", _ctx->errstr);
        redisFree(_ctx);
    } else {
        printf("connect error: connect failed\n");
    }

    return false;
}

#endif //CONNECTIONPOOL_REDIS_CONNECTION_H
