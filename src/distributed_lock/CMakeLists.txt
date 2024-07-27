
find_package(connection_pool 1.0.0 QUIET)
if (NOT connection_pool_FOUND)
    include(FetchContent)
    fetchcontent_declare(connection_pool
        GIT_REPOSITORY https://github.com/Portgas-D-Asce/ConnectionPool.git
        GIT_TAG v1.0.0-alpha
    )
    fetchcontent_makeavailable(connection_pool)
    # 拉取失败
    if(NOT connection_pool_POPULATED)
        message(FATAL_ERROR "fetch connection_pool failed!")
    endif ()
endif ()

add_library(redis_lock SHARED)
add_library(distributed_lock::redis_lock ALIAS redis_lock)

target_sources(redis_lock
    PRIVATE
    redis_distributed_lock.cpp
)

target_include_directories(redis_lock
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(redis_lock
    PUBLIC
    connection_pool::redis_pool
)

set_target_properties(redis_lock
    PROPERTIES
    POSITION_INDEPENDENT_CODE 1
    SOVERSION ${PROJECT_VERSION}
    OUTPUT_NAME "redis_lock"
    DEBUG_POSTFIX "_d"
    PUBLIC_HEADER "redis_distributed_lock.h"
    MACOSX_RPATH ON
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

install(
    TARGETS
    redis_lock
    EXPORT
    ${PROJECT_NAME}-targets
    ARCHIVE
    DESTINATION ${INSTALL_LIBDIR}
    COMPONENT lib
    RUNTIME
    DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
    LIBRARY
    DESTINATION ${INSTALL_LIBDIR}
    COMPONENT lib
    PUBLIC_HEADER
    DESTINATION ${INSTALL_INCLUDEDIR}/distributed_lock
    COMPONENT dev
)
