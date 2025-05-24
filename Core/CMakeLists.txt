cmake_minimum_required(VERSION 3.18)
project(StalkerCore LANGUAGES CXX)

################################################################################
# SECTION: SIMD options and defaults
################################################################################

# SIMD: Option to enable/disable
if(NOT DEFINED STALKER_ENABLE_SIMD)
    option(STALKER_ENABLE_SIMD "Enable SIMD support" ON)
    message(STATUS "STALKER_ENABLE_SIMD not defined, setting default to ON")
endif()

# SIMD: Instruction set choice (only set if SIMD is ON and not already set)
if(STALKER_ENABLE_SIMD)
    if(NOT DEFINED STALKER_SIMD_INSTRUCTION_SET)
        set(STALKER_SIMD_INSTRUCTION_SET "avx2" CACHE STRING "SIMD instruction set (avx2, avx512, esp-dsp, none)")
        message(STATUS "STALKER_SIMD_INSTRUCTION_SET not defined, setting default to avx2")
    endif()
endif()

# SIMD: Unroll factor
if(NOT DEFINED STALKER_UNROLL_FACTOR)
    set(STALKER_UNROLL_FACTOR 4 CACHE STRING "Loop unroll factor for Stalker SIMD code")
endif()
message(STATUS "STALKER_UNROLL_FACTOR: ${STALKER_UNROLL_FACTOR}")

################################################################################
# SECTION: Parallelization (OpenMP & Internal Threading)
################################################################################

option(STALKER_ENABLE_OPENMP "Enable OpenMP parallelization" OFF)
option(STALKER_ENABLE_INTERNAL_THREADING "Enable internal thread pool instead of OpenMP" OFF)

# Handle OpenMP (find package and link)
set(_use_openmp FALSE)
if(STALKER_ENABLE_OPENMP)
    find_package(OpenMP REQUIRED)
    message(STATUS "OpenMP enabled")
    set(_use_openmp TRUE)
endif()

if(STALKER_ENABLE_INTERNAL_THREADING)
    message(STATUS "Internal thread pool enabled, OpenMP will be disabled")
    set(_use_openmp FALSE)
endif()

################################################################################
# SECTION: Target Declaration
################################################################################

add_library(StalkerCore INTERFACE)

################################################################################
# SECTION: Target Configuration (INCLUDES, DEFINITIONS, OPTIONS, LIBS)
################################################################################

# Includes for install/build
target_include_directories(StalkerCore INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

# Core compile definitions
target_compile_definitions(StalkerCore INTERFACE
    STALKER_UNROLL_FACTOR=${STALKER_UNROLL_FACTOR}
    $<$<BOOL:${STALKER_ENABLE_SIMD}>:STALKER_ENABLE_SIMD=1>
    $<$<BOOL:${_use_openmp}>:STALKER_ENABLE_OPENMP=1>
    $<$<BOOL:${STALKER_ENABLE_INTERNAL_THREADING}>:STALKER_ENABLE_INTERNAL_THREADING=1>
)

# SIMD instruction set macros and flags
if(STALKER_ENABLE_SIMD)
    if(STALKER_SIMD_INSTRUCTION_SET STREQUAL "avx2")
        target_compile_definitions(StalkerCore INTERFACE STALKER_SIMD_INSTRUCTION_SET_AVX2)
        target_compile_options(StalkerCore INTERFACE
            $<$<CXX_COMPILER_ID:GNU>:-mavx2 -mfma>
            $<$<CXX_COMPILER_ID:Clang>:-mavx2 -mfma>
            $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
        )
    elseif(STALKER_SIMD_INSTRUCTION_SET STREQUAL "avx512")
        target_compile_definitions(StalkerCore INTERFACE STALKER_SIMD_INSTRUCTION_SET_AVX512)
        target_compile_options(StalkerCore INTERFACE
            $<$<CXX_COMPILER_ID:GNU>:-mavx512f>
            $<$<CXX_COMPILER_ID:Clang>:-mavx512f>
            $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX512>
        )
    elseif(STALKER_SIMD_INSTRUCTION_SET STREQUAL "esp-dsp")
        target_compile_definitions(StalkerCore INTERFACE STALKER_SIMD_INSTRUCTION_SET_ESP_DSP)
        # No standard flags for esp-dsp
    elseif(STALKER_SIMD_INSTRUCTION_SET STREQUAL "none")
        target_compile_definitions(StalkerCore INTERFACE STALKER_SIMD_INSTRUCTION_SET_NONE)
    else()
        message(WARNING "Unknown SIMD instruction set: ${STALKER_SIMD_INSTRUCTION_SET}")
        target_compile_definitions(StalkerCore INTERFACE STALKER_SIMD_INSTRUCTION_SET_NONE)
    endif()
endif()

# Hiph Performance flags
option(STALKER_ALL_GAS_NO_BRAKES "Enable maximum performance compiler flags" OFF)

if(STALKER_ALL_GAS_NO_BRAKES)
    message(STATUS "ALL GAS NO BRAKES: Enabling max optimization flags: -O3 -ffast-math -march=native -funroll-loops")
    target_compile_options(StalkerCore INTERFACE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -ffast-math -march=native -funroll-loops>
        $<$<CXX_COMPILER_ID:MSVC>:/Ox /fp:fast>
    )
endif()

# OpenMP linking (if enabled and not overridden)
if(_use_openmp)
    target_link_libraries(StalkerCore INTERFACE OpenMP::OpenMP_CXX)
endif()

################################################################################
# SECTION: Install Logic
################################################################################

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/Stalker/Core
    DESTINATION include/Stalker
)

install(TARGETS StalkerCore
    EXPORT StalkerTargets
    INCLUDES DESTINATION include
)

################################################################################
# SECTION: Diagnostics
################################################################################

message(STATUS "STALKER_ENABLE_SIMD: ${STALKER_ENABLE_SIMD}")
message(STATUS "STALKER_SIMD_INSTRUCTION_SET: ${STALKER_SIMD_INSTRUCTION_SET}")
message(STATUS "STALKER_ENABLE_OPENMP: ${STALKER_ENABLE_OPENMP}")
message(STATUS "STALKER_ENABLE_INTERNAL_THREADING: ${STALKER_ENABLE_INTERNAL_THREADING}")
