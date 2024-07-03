#ifndef CONNECTIONPOOL_CONNECTION_H
#define CONNECTIONPOOL_CONNECTION_H
#include <memory>
#include <chrono>

class Connection {
private:
    // 最后一次被使用的时间点
    std::chrono::system_clock::time_point _last_active;
    // 控制智能指针是否释放资源
    bool _flag;

public:
    explicit Connection() : _last_active(std::chrono::system_clock::now()), _flag(true) {}

    virtual ~Connection() = default;

    // 禁止拷贝构造，否则 redis/mysql 上下文会二次释放
    Connection(Connection&) = delete;
    Connection& operator=(Connection&) = delete;

    virtual bool connect(const struct timeval& timeout) = 0;

    bool flag() const {
        return _flag;
    }

    void flag(bool flg) {
        _flag = flg;
    }

    void last_active(std::chrono::system_clock::time_point tp) {
        _last_active = tp;
    }

    long long idle() {
        std::chrono::nanoseconds idle = std::chrono::system_clock::now() - _last_active;
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(idle);
        return ms.count();
    }

    // 自定义智能指针释放函数
    static void deleter(Connection* p) {
        if(p->_flag) {
            printf("delete connection* p\n");
            delete p;
        } else {
            printf("do nothing when deleting connection* p\n");
        }
    }
};

#endif //CONNECTIONPOOL_CONNECTION_H
