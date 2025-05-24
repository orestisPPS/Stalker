# Header-only library for Memory module
add_library(StalkerMemory INTERFACE)

# Dependencies (Core is always required)
target_link_libraries(StalkerMemory INTERFACE StalkerCore)

# Public include path for this module (relative to project root)
target_include_directories(StalkerMemory INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

# Install the headers for this module
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/Stalker/Memory
    DESTINATION include/Stalker
)

# Export for find_package usage if you're setting up full package config
install(TARGETS StalkerMemory
    EXPORT StalkerTargets
    INCLUDES DESTINATION include
)
