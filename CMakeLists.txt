cmake_minimum_required(VERSION 3.18)
if(NOT DEFINED ESP_PLATFORM)
    project(Stalker VERSION 0.1.0 LANGUAGES CXX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optional: speed up local rebuilds via ccache when available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "ccache detected: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Compiler version check
if(CMAKE_CXX_COMPILER_ID)
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
endif()


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(${CMAKE_CURRENT_LIST_DIR}/cmake/Options.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/StalkerModules.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/DetectPlatform.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ConfigureAlignment.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/DetectSIMD.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/DetectThreads.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ConfigureCompilerFlags.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ConfigureUnroll.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ESP32Integration.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/CoreTarget.cmake)

function(stalker_summary_print)
    message(STATUS "")
    message(STATUS "=========================================")
    message(STATUS "========== STALKER BUILD CONFIG ==========")
    message(STATUS "==========================================")
    message(STATUS "")
    get_cmake_property(_variableNames CACHE_VARIABLES)
    foreach(_vn IN LISTS _variableNames)
        if(_vn MATCHES "^STALKER_")
            message(STATUS "${_vn} = ${${_vn}}")
        endif()
    endforeach()
    if(DEFINED STALKER_EFFECTIVE_COMPILE_FLAGS)
        string(REPLACE ";" " " _flags_str "${STALKER_EFFECTIVE_COMPILE_FLAGS}")
        message(STATUS "Effective compile flags: ${_flags_str}")
    endif()
endfunction()

function(stalker_print_summary)
    stalker_summary_print()
endfunction()

# ------------------------------------------------------------
# Consolidated detection previously in Core/CMakeLists.txt
# ------------------------------------------------------------
stalker_platform_detect()
stalker_alignment_init()
stalker_simd_init()
stalker_threads_init()
stalker_unroll_init()

if(NOT CMAKE_CONFIGURATION_TYPES)
    if(STALKER_BUILD_PROFILE STREQUAL "debug")
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    elseif(STALKER_BUILD_PROFILE STREQUAL "relwithdebinfo")
        set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type" FORCE)
    elseif(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
        # Generally default to Release when not specified by the user
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    endif()
endif()

stalker_compiler_flags_init(_stalkerCompilerFlags)

if(NOT CMAKE_SCRIPT_MODE_FILE)
    add_library(StalkerCore INTERFACE)
    stalker_core_target_init(StalkerCore)

    stalker_modules_define_deps()
    stalker_modules_expand("${STALKER_MODULES}" _effective_modules)
    stalker_modules_realize("${_effective_modules}")

    # Create interface library for the complete Stalker library
    add_library(Stalker INTERFACE)
    target_link_libraries(Stalker INTERFACE
        StalkerCore
        $<$<TARGET_EXISTS:StalkerMemory>:StalkerMemory>
        $<$<TARGET_EXISTS:StalkerUtility>:StalkerUtility>
        $<$<TARGET_EXISTS:StalkerContainers>:StalkerContainers>
        $<$<TARGET_EXISTS:StalkerMathematics>:StalkerMathematics>
        $<$<TARGET_EXISTS:StalkerThreading>:StalkerThreading>
    )

    # Optional in-tree alias for namespaced usage
    add_library(Stalker::Stalker ALIAS Stalker)
endif()

# Bridge into ESP-IDF component builds when applicable
set(STALKER_ESP32_COMPONENT_ACTIVE FALSE)
stalker_esp32_component_init()

if(STALKER_ESP32_COMPONENT_ACTIVE)
    message(STATUS "Stalker registered as ESP-IDF component; skipping host target configuration.")
    return()
endif()


# Always build tests and benchmarks by default
option(STALKER_BUILD_TESTS "Build tests" OFF)
option(STALKER_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(STALKER_BUILD_EXAMPLES "Build examples" OFF)
option(STALKER_BUILD_PYTHON "Build Python bindings" OFF)

if(STALKER_BUILD_PYTHON)
    add_subdirectory(bindings/python)
endif()

if(STALKER_BUILD_TESTS)
    add_subdirectory(Tests)
endif()

if(STALKER_BUILD_BENCHMARKS)
    add_subdirectory(Benchmarks)
endif()

if(STALKER_BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()

if(NOT CMAKE_SCRIPT_MODE_FILE)
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

# Diagnostics (single summary call)
stalker_summary_print()


configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/StalkerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/StalkerConfig.cmake
    INSTALL_DESTINATION lib/cmake/Stalker
)

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/StalkerConfigVersion.cmake
        VERSION 0.1.0
        COMPATIBILITY SameMajorVersion
    )
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/StalkerConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/StalkerConfigVersion.cmake
    DESTINATION lib/cmake/Stalker
)
endif()
