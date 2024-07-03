cmake_minimum_required(VERSION 3.26)
project(DistributedLock)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include
        src/connection_pool
        src/singleton
        src/distributed_lock
        src/factory
        src/timer)

link_directories(/usr/local/lib)

add_executable(DistributedLock src/main.cpp)

target_link_libraries(DistributedLock hiredis)
