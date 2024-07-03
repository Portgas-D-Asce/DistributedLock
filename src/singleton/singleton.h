#ifndef CONNECTIONPOOL_SINGLETON_H
#define CONNECTIONPOOL_SINGLETON_H
#include <memory>
#include <thread>

template<typename T>
class Singleton {
public:
    static T& get_instance() {
        static std::once_flag flag;
        std::call_once(flag, [&]() {
            _instance = std::unique_ptr<T>(new T());
        });
        return *_instance;
    }
private:
    static std::unique_ptr<T> _instance;
};

template<typename T>
std::unique_ptr<T> Singleton<T>::_instance = nullptr;

#endif //CONNECTIONPOOL_SINGLETON_H
