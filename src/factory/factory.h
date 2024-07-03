#ifndef REDISDISTRIBUTEDLOCK_FACTORY_H
#define REDISDISTRIBUTEDLOCK_FACTORY_H
#include <utility>

template<typename Product>
class Factory {
public:
    template<typename... Args>
    static Product create(Args&&... args) {
        return Product(std::forward<Args>(args)...);
    }
};

#endif //REDISDISTRIBUTEDLOCK_FACTORY_H
