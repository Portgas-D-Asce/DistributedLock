
add_library(timer INTERFACE)

file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
set_target_properties(timer
    PROPERTIES
    # 会导致只安装一个文件，后续文件不会被安装
    # PUBLIC_HEADER ${headers}
    PUBLIC_HEADER "${headers}"
)

install(
    TARGETS
    timer
    PUBLIC_HEADER
    DESTINATION ${INSTALL_INCLUDEDIR}/timer
    COMPONENT dev
)