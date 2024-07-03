#ifndef CONNECTIONPOOL_REDIS_CONNECTION_OPTION_H
#define CONNECTIONPOOL_REDIS_CONNECTION_OPTION_H
#include <string>
#include <memory>
#include "connection_option.h"

class RedisConnectionOption : public ConnectionOption {
public:
    explicit RedisConnectionOption(std::string host = "127.0.0.1", size_t port = 6379,
                                   std::string user = "", std::string passwd = "")
        : ConnectionOption(std::move(host), port, std::move(user), std::move(passwd)) {}
};


#endif //CONNECTIONPOOL_REDIS_CONNECTION_OPTION_H
