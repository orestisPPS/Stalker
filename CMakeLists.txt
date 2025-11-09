cmake_minimum_required(VERSION 3.18)
project(Stalker VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler version check
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
        message(FATAL_ERROR "GCC 7.0 or newer required (for C++17: if constexpr, index_sequence, etc)")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
        message(FATAL_ERROR "Clang 7.0 or newer required (for C++17 features)")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.12)
        message(FATAL_ERROR "MSVC 19.12 (VS2017 15.5) or newer required (for C++17 features)")
    endif()
else()
    message(WARNING "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}. C++17 support not guaranteed.")
endif()


add_subdirectory(Core)
add_subdirectory(Memory)
add_subdirectory(Utility)
add_subdirectory(Mathematics)
add_subdirectory(Threading)

# Create interface library for the complete Stalker library
add_library(Stalker INTERFACE)
target_link_libraries(Stalker INTERFACE
    StalkerCore
    StalkerMemory
    StalkerUtility
    StalkerMathematics
    StalkerThreading
)

# Optional in-tree alias for namespaced usage
add_library(Stalker::Stalker ALIAS Stalker)


# Always build tests and benchmarks by default
option(STALKER_BUILD_TESTS "Build tests" OFF)
option(STALKER_BUILD_BENCHMARKS "Build benchmarks" OFF)

if(STALKER_BUILD_TESTS)
    add_subdirectory(Tests)
endif()

if(STALKER_BUILD_BENCHMARKS)
    add_subdirectory(Benchmarks)
endif()

# -------------------------
# Install and CMake package
# -------------------------
include(CMakePackageConfigHelpers)

# Install umbrella INTERFACE target into export set
install(TARGETS Stalker
    EXPORT StalkerTargets
    INCLUDES DESTINATION include
)

# Export all targets (modules add to the same export in their CMakeLists)
install(
    EXPORT StalkerTargets
    NAMESPACE Stalker::
    DESTINATION lib/cmake/Stalker
)

# Configure and install package config files
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Tools/cmake/StalkerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/StalkerConfig.cmake
    INSTALL_DESTINATION lib/cmake/Stalker
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/StalkerConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/StalkerConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/StalkerConfigVersion.cmake
    DESTINATION lib/cmake/Stalker
)