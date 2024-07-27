# 设置编译模式为 release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")


# 生成目录、安装目录均存放在顶层，方便查看、运行生成文件
# 启用 GNUInstallDirs 模块
include(GNUInstallDirs)
# 设置目标生成目录
set(TARGET_DIR ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_DIR}/${CMAKE_INSTALL_BINDIR})
# set(LIBRARY_OUTPUT_PATH ${TARGET_DIR}/${CMAKE_INSTALL_LIBDIR} CACHE PATH "lib output" FORCE)
# set(EXECUTABLE_OUTPUT_PATH ${TARGET_DIR}/${CMAKE_INSTALL_BINDIR} CACHE PATH "bin output" FORCE)


# 设置安装目录，使用全局设置是因为 CMAKE_INSTALL_PREFIX 可能会失效
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

# 定义 4 个全局目录 INSTALL_LIBDIR、INSTALL_BINDIR、INSTALL_INCLUDEDIR、INSTALL_CMAKEDIR
# Offer the user the choice of overriding the installation directories
set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
set(INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
# 设置 CMAKE 安装目录
if(WIN32 AND NOT CYGWIN)
    set(DEF_INSTALL_CMAKEDIR CMake)
else()
    set(DEF_INSTALL_CMAKEDIR share/cmake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKEDIR ${DEF_INSTALL_CMAKEDIR})

# 打印所有类型安装目录
# message(STATUS "Project will be installed to ${CMAKE_INSTALL_PREFIX}")
# foreach(p LIB BIN INCLUDE CMAKE)
#     file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${INSTALL_${p}DIR} _path)
#     message(STATUS "Installing ${p} components to ${_path}")
#     unset(_path)
# endforeach()
