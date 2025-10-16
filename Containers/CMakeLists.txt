add_library(StalkerContainers INTERFACE)

target_include_directories(StalkerContainers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(StalkerContainers INTERFACE StalkerCore)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/Stalker/Containers
    DESTINATION include/Stalker
)

install(TARGETS StalkerContainers
    EXPORT StalkerTargets
    INCLUDES DESTINATION include
)
