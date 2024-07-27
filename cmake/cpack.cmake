include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "Hello World")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/INSTALL.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "distributed lock: a generic distributed lock")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

# 前面斜杠是必须的，否则会生成空压缩包
set(CPACK_PACKAGING_INSTALL_PREFIX "/${PROJECT_NAME}")

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_SOURCE_IGNORE_FILES ${PROJECT_BINARY_DIR} "cmake-build-debug"
    "cmake-build-centos" ".git" ".gitignore" ".idea" ".DS_Store")

# 源码包
set(CPACK_SOURCE_GENERATOR "ZIP;TGZ")

# 二进制包
set(CPACK_GENERATOR "ZIP;TGZ")

#[[if(UNIX)
    if(CMAKE_SYSTEM_NAME MATCHES Linux)
        list(APPEND CPACK_GENERATOR "DEB")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "robertodr")
        set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "uuid-dev")

        list(APPEND CPACK_GENERATOR "RPM")
        set(CPACK_RPM_PACKAGE_RELEASE "1")
        set(CPACK_RPM_PACKAGE_LICENSE "MIT")
        set(CPACK_RPM_PACKAGE_REQUIRES "uuid-devel")
    endif()
endif()

if(WIN32 OR MINGW)
    list(APPEND CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "message")
    set(CPACK_NSIS_CONTACT "robertdr")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
endif()

if(APPLE)
    list(APPEND CPACK_GENERATOR "Bundle")
    set(CPACK_BUNDLE_NAME "message")
    configure_file(${PROJECT_SOURCE_DIR}/cmake/Info.plist.in Info.plist @ONLY)
    set(CPACK_BUNDLE_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
    set(CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/cmake/coffee.icns)
endif()]]

message(STATUS "CPack generators: ${CPACK_GENERATOR}")

include(CPack)
