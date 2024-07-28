

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})

find_package(distributed_lock REQUIRED)

add_executable(redis_lock_demo)
target_sources(redis_lock_demo
    PRIVATE
    main.cpp
)

target_link_libraries(redis_lock_demo
    PRIVATE
    distributed_lock::redis_lock
)

#[[
file(RELATIVE_PATH _rel ${CMAKE_INSTALL_PREFIX}/${INSTALL_BINDIR} ${CMAKE_INSTALL_PREFIX})
if(APPLE)
    set(_rpath "@loader_path/${_rel}")
else()
    set(_rpath "\$ORIGIN/${_rel}")
endif()
file(TO_NATIVE_PATH "${_rpath}/${INSTALL_LIBDIR}" exec_rpath)

set_target_properties(redis_lock_demo
    PROPERTIES
    MACOSX_RPATH ON
    SKIP_BUILD_RPATH OFF
    BUILD_WITH_INSTALL_RPATH OFF
    INSTALL_RPATH ${exec_rpath}
    INSTALL_RPATH_USE_LINK_PATH ON
)

# 安装
install(
    TARGETS
    redis_lock_demo
    RUNTIME
    DESTINATION ${INSTALL_BINDIR}
    COMPONENT bin
)
]]
