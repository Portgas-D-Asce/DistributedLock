#include <iostream>
#include <cstdlib>
#include <cassert>
#include <unistd.h>
#include <hiredis/hiredis.h>
#include "redis_connection.h"
#include "connection_pool.h"
#include "redis_distributed_lock.h"
#include "factory.h"
using namespace std;
using Pool = ConnectionPool<RedisConnection>;


template<typename DistributedLock>
void func(DistributedLock&& dlk) {
    atomic<int> x = 0;
    auto routing = [&]() {
        auto conn = Singleton<Pool>::get_instance().get();
        if(!conn) {
            printf("get connection failed\n");
            return;
        }
        auto c = conn->ctx();
        redisReply* reply;
        for(int i = 0; i < 50; ++i) {
            while(!dlk.try_lock()) {
                usleep(100);
            }
            // for(int j = 0; j < 5; ++j) dlk.try_lock();

            reply = static_cast<redisReply *>(redisCommand(c, "GET counter"));
            int cnt = stoi(reply->str);
            printf("get counter: %d\n", cnt);
            freeReplyObject(reply);
            cnt--;

            reply = static_cast<redisReply *>(redisCommand(c, "SET counter %s", to_string(cnt).c_str()));
            printf("new counter: %s\n", reply->str);
            freeReplyObject(reply);

            // for(int j = 0; j < 5; ++j) dlk.unlock();
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

    func(Factory<RedisDistributedLock>::create("xxx", 20));

    if(cid != 0) {
        waitpid(cid, nullptr, 0);
    }

    return 0;
}
