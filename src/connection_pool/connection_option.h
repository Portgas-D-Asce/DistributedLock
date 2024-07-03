#ifndef CONNECTIONPOOL_CONNECTION_OPTION_H
#define CONNECTIONPOOL_CONNECTION_OPTION_H
#include <string>
#include <utility>

class ConnectionOption {
private:
    std::string _host;
    size_t _port;
    std::string _user;
    std::string _passwd;
public:
    ConnectionOption(std::string host, size_t port, std::string user, std::string passwd)
        : _host(std::move(host)), _port(port), _user(std::move(user)), _passwd(std::move(passwd)) {}

    virtual ~ConnectionOption() = default;

    std::string host() const {
        return _host;
    }

    size_t port() const {
        return _port;
    }

    std::string user() const {
        return _user;
    }

    std::string passwd() const {
        return _passwd;
    }
};


#endif //CONNECTIONPOOL_CONNECTION_OPTION_H
