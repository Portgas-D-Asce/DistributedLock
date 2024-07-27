cmake_minimum_required(VERSION 3.20)

project(distributed_lock VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/base.cmake)

add_subdirectory(src)

# add_subdirectory(example)

include(cmake/cmake.cmake)

include(cmake/cpack.cmake)
