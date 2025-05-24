# Utility/CMakeLists.txt

add_library(StalkerUtility INTERFACE)

# If you use Core features (which you do: error types, maybe units, etc.)
target_link_libraries(StalkerUtility INTERFACE StalkerCore)

# Include Utility headers for consumers (header-only)
target_include_directories(StalkerUtility INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

# Install only the Utility headers
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/Stalker/Utility
    DESTINATION include/Stalker
)

# Export target for package consumers
install(TARGETS StalkerUtility
    EXPORT StalkerTargets
    INCLUDES DESTINATION include
)
