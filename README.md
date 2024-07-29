# DistributedLock

基于 Redis Hash 类型实现的 “支持防死锁，自动续期的分布式递归锁”。

# dependency
necessary
- [connection_pool](https://github.com/Portgas-D-Asce/ConnectionPool)

optional
- [googletest](https://github.com/google/googletest)

# usage
```cmake
find_package(distributed_lock 1.0.0 QUIET)
if (NOT distributed_lock_FOUND)
    include(FetchContent)
    fetchcontent_declare(distributed_lock
        GIT_REPOSITORY https://github.com/Portgas-D-Asce/DistributedLock.git
        GIT_TAG v1.0.0-alpha
    )
    fetchcontent_makeavailable(distributed_lock)
    # 拉取失败
    if(NOT distributed_lock_POPULATED)
        message(FATAL_ERROR "fetch distributed_lock failed!")
    endif ()
endif ()

target_link_libraries(exec
    PUBLIC
    $<$<BOOL:${distributed_lock_FOUND}>:distributed_lock::>redis_lock
)
```