# 安装 targets、targets-release 文件
install(
    EXPORT
    # 导出目标
    ${PROJECT_NAME}-targets
    NAMESPACE
    # 命名空间
    "${PROJECT_NAME}::"
    DESTINATION
    # 安装目录
    ${INSTALL_CMAKEDIR}
    COMPONENT
    dev
)

# 生成 config-version 文件，临时输出使用相对路径
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    # 输出目录
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 生成 config 文件，临时输出使用相对路径
configure_package_config_file(
    # config 文件模版
    ${CMAKE_CURRENT_LIST_DIR}/config.cmake.in
    # 生成代码位置
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    # 必要参数，只是作为参数传入，不会真正安装
    INSTALL_DESTINATION ${INSTALL_CMAKEDIR}
)

# 安装 config、config-version 文件
install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    DESTINATION
    ${INSTALL_CMAKEDIR}
)
