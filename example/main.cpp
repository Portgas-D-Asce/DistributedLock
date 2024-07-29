#include <iostream>
#include <cstdlib>
#include <cassert>
#include <sys/wait.h>
#include <unistd.h>

#include "distributed_lock/redis_distributed_lock.h"
#include "factory/factory.h"
using namespace std;

// #define TEST_RENEWAL
// #define TEST_RECURSIVE

template<typename DistributedLock>
void func(DistributedLock&& dlk) {

    auto routing = [&]() {
        auto conn = SingleConnectionPool<RedisConnection>::get_instance().get();
        assert(conn && "get connection failed!");
        auto c = conn->ctx();
        redisReply* reply;
        for(int i = 0; i < 500; ++i) {
            while(!dlk.try_lock()) {
                usleep(100);
            }

#ifdef TEST_RECURSIVE
            for(int j = 0; j < 5; ++j) dlk.try_unlock();
#endif

            reply = static_cast<redisReply *>(redisCommand(c, "GET counter"));
            int cnt = stoi(reply->str);
            printf("get counter: %d\n", cnt);
            freeReplyObject(reply);
            cnt--;

            reply = static_cast<redisReply *>(redisCommand(c, "SET counter %s", to_string(cnt).c_str()));
            printf("new counter: %s\n", reply->str);
            freeReplyObject(reply);
#ifdef TEST_RENEWAL
            sleep(120);
#endif

#ifdef TEST_RECURSIVE
            for(int j = 0; j < 5; ++j) dlk.unlock();
#endif
            dlk.unlock();
        }
    };

    int m = 10;
    vector<shared_ptr<thread>> ts(m);
    for(int i = 0; i < m; ++i) {
        ts[i] = make_shared<thread>(routing);
    }

    for(int i = 0; i < m; ++i) {
        ts[i]->join();
    }
}

int main() {
    int cid = fork();
    if(cid == -1) {
        printf("fork error!\n");
        exit(1);
    }

    int ccid = fork();
    if(ccid == -1) {
        printf("fork error!\n");
        exit(1);
    }

    if(ccid != 0) {
        waitpid(ccid, nullptr, 0);
    }
    // init connection pool, we can use "get_instance()" to get singleton instance after this.
    // don't keep a shared_ptr, destroy will in endless loop
    std::string redis_config = "/Users/pk/Project/DistributedLock/config/redis_config.toml";
    SingleConnectionPool<RedisConnection>::get_instance(redis_config).get();


    func(Factory<RedisDistributedLock>::create("xxx", 20));

    if(cid != 0) {
        waitpid(cid, nullptr, 0);
    }

    // 手动释放单例，必须保证所有单例引用已销毁，否则会陷入死循环
    SingleConnectionPool<RedisConnection>::destroy();
    return 0;
}
