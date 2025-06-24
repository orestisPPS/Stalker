 cmake_minimum_required(VERSION 3.18)
project(StalkerCore LANGUAGES CXX)

################################################################################
# PLATFORM DETECTION
################################################################################
# Detect known supported platforms: Windows, Linux, ESP32-S3 (and generic ESP)
if(UNIX AND NOT APPLE)
    set(STALKER_PLATFORM "LINUX" CACHE STRING "Platform: WINDOWS, LINUX" FORCE)
elseif(WIN32)
    set(STALKER_PLATFORM "WINDOWS" CACHE STRING "Platform: WINDOWS, LINUX" FORCE)
else()
    message(FATAL_ERROR "Unsupported platform. Only Windows, Linux are supported.")
endif()

################################################################################
# CACHE LINE AND BITNESS DETECTION
################################################################################
# Detect bitness (32/64)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(STALKER_PLATFORM_BITS "64" CACHE STRING "Platform bitness: 64 or 32" FORCE)
    set(STALKER_PLATFORM_CACHE_LINE_SIZE "64" CACHE STRING "Cache line size in bytes" FORCE)
else()
    set(STALKER_PLATFORM_BITS "32" CACHE STRING "Platform bitness: 64 or 32" FORCE)
    set(STALKER_PLATFORM_CACHE_LINE_SIZE "32" CACHE STRING "Cache line size in bytes" FORCE)
endif()

################################################################################
# ALIGNMENT CONFIGURATION
################################################################################
# Default alignment is cache line size, but user can override
if(NOT DEFINED STALKER_ALIGNMENT)
    set(STALKER_ALIGNMENT ${STALKER_PLATFORM_CACHE_LINE_SIZE} CACHE STRING "Data alignment in bytes (default: ${STALKER_PLATFORM_CACHE_LINE_SIZE})" FORCE)
    message(STATUS "STALKER_ALIGNMENT was not set by the user; defaulting to cache line size = ${STALKER_ALIGNMENT} bytes")
endif()

# Check if STALKER_ALIGNMENT is a positive power of 2
math(EXPR _align_minus_one "${STALKER_ALIGNMENT} - 1")
math(EXPR _align_and "${STALKER_ALIGNMENT} & ${_align_minus_one}")
if(NOT "${STALKER_ALIGNMENT}" GREATER 0 OR NOT "${_align_and}" EQUAL 0)
    message(FATAL_ERROR
        "STALKER_ALIGNMENT (${STALKER_ALIGNMENT}) must be a positive power of 2. "
        "Set it with: -DSTALKER_ALIGNMENT=VALUE"
    )
endif()

################################################################################
# SIMD CONFIGURATION
################################################################################

# Option to enable/disable SIMD (user can override)
option(STALKER_SIMD_ENABLE "Enable SIMD support" ON)

# If SIMD is disabled by user, just force everything to OFF/none
if(NOT STALKER_SIMD_ENABLE)
    set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "" FORCE)
    set(_simd_ok 0)
else()
    # Hardware detection and instruction set negotiation
    enable_language(C)
    include(CheckCSourceRuns)

    # Probe for AVX512
    set(CMAKE_REQUIRED_FLAGS "-mavx512f")
    check_c_source_runs("
    #include <immintrin.h>
    int main() {
        __m512i x = _mm512_set1_epi32(0);
        (void)x;
        return 0;
    }
    " HAS_AVX512F)

    # Probe for AVX2
    set(CMAKE_REQUIRED_FLAGS "-mavx2")
    check_c_source_runs("
    #include <immintrin.h>
    int main() {
        __m256i x = _mm256_set1_epi32(0);
        (void)x;
        return 0;
    }
    " HAS_AVX2)

    unset(CMAKE_REQUIRED_FLAGS)

    # Choose instruction set (user-overridable, default auto)
    set(STALKER_SIMD_INSTRUCTION_SET "auto" CACHE STRING "SIMD instruction set: avx2, avx512, auto, esp-dsp, none")

    set(_simd_ok 0)
    if(STALKER_SIMD_INSTRUCTION_SET STREQUAL "avx512")
        if(HAS_AVX512F)
            set(_simd_ok 1)
        elseif(HAS_AVX2)
            message(WARNING "Requested AVX512, but hardware doesn't support it. Falling back to AVX2.")
            set(STALKER_SIMD_INSTRUCTION_SET "avx2" CACHE STRING "" FORCE)
            set(_simd_ok 1)
        else()
            message(WARNING "Requested AVX512, but hardware doesn't support any of the vectorization methods of the library. Falling back to no SIMD.")
            set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "" FORCE)
        endif()
    elseif(STALKER_SIMD_INSTRUCTION_SET STREQUAL "avx2")
        if(HAS_AVX2)
            set(_simd_ok 1)
        else()
            message(WARNING "Requested AVX2, but hardware doesn't support any of the vectorization methods of the library. Falling back to no SIMD.")
            set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "" FORCE)
        endif()
    elseif(STALKER_SIMD_INSTRUCTION_SET STREQUAL "auto")
        if(HAS_AVX512F)
            set(STALKER_SIMD_INSTRUCTION_SET "avx512" CACHE STRING "" FORCE)
            set(_simd_ok 1)
        elseif(HAS_AVX2)
            set(STALKER_SIMD_INSTRUCTION_SET "avx2" CACHE STRING "" FORCE)
            set(_simd_ok 1)
        else()
            set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "" FORCE)
            set(_simd_ok 0)
        endif()
    elseif(STALKER_SIMD_INSTRUCTION_SET STREQUAL "esp-dsp")
        message(FATAL_ERROR "SIMD instruction set 'esp-dsp' is not supported in this configuration.")
        set(_simd_ok 0)
    elseif(STALKER_SIMD_INSTRUCTION_SET STREQUAL "none")
        set(_simd_ok 0)
    else()
        message(WARNING "Unknown SIMD instruction set: ${STALKER_SIMD_INSTRUCTION_SET}. Falling back to no SIMD.")
        set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "" FORCE)
        set(_simd_ok 0)
    endif()

    if(NOT _simd_ok)
        set(STALKER_SIMD_ENABLE OFF CACHE BOOL "" FORCE)
        message(WARNING "SIMD is disabled due to unsupported instruction set: ${STALKER_SIMD_INSTRUCTION_SET}.")
        set(STALKER_SIMD_INSTRUCTION_SET "none" CACHE STRING "" FORCE)
    endif()
endif()

if(STALKER_SIMD_ENABLE)
    set (STALKER_SIMD_STORE_POLICY "stream" CACHE STRING "SIMD store policy: stream, cache")
    if(NOT STALKER_SIMD_STORE_POLICY STREQUAL "stream" AND NOT STALKER_SIMD_STORE_POLICY STREQUAL "cache")
        message(FATAL_ERROR "Invalid STALKER_SIMD_STORE_POLICY: ${STALKER_SIMD_STORE_POLICY}. Must be 'stream' or 'cache'.")
    endif()
endif()
     

################################################################################
# LOOP UNROLLING CONFIGURATION
################################################################################

# Enable or disable loop unrolling (user-overridable)
option(STALKER_UNROLL_ENABLE "Enable loop unrolling in SIMD code" ON)

if(STALKER_UNROLL_ENABLE)
    if(NOT DEFINED STALKER_UNROLL_FACTOR)
        set(STALKER_UNROLL_FACTOR 4 CACHE STRING "Loop unroll factor for SIMD" FORCE)
    else()
        # Validate unroll factor
        if(NOT STALKER_UNROLL_FACTOR GREATER 0)
            message(FATAL_ERROR "STALKER_UNROLL_FACTOR must be a positive integer. Set it with: -DSTALKER_UNROLL_FACTOR=VALUE")
        endif()
        if(STALKER_UNROLL_FACTOR EQUAL 1)
            message(WARNING "STALKER_UNROLL_FACTOR=1 is effectively disabling unrolling. Consider setting STALKER_UNROLL_ENABLE=OFF instead.")
        endif()
    endif()
else()
    # Always enforce unroll factor = 1 when unrolling is off, regardless of user setting
    if(DEFINED STALKER_UNROLL_FACTOR AND NOT (STALKER_UNROLL_FACTOR STREQUAL "1"))
        message(WARNING "STALKER_UNROLL_ENABLE=OFF. Defaulting STALKER_UNROLL_FACTOR to 1 (was previously set to ${STALKER_UNROLL_FACTOR}).")
        set(STALKER_UNROLL_FACTOR 1 CACHE STRING "Loop unroll factor for SIMD" FORCE)
    endif()
endif()


################################################################################
# THREADING CONFIGURATION
################################################################################

option(STALKER_THREADING_ENABLE "Enable multithreading support" ON)
option(STALKER_THREADING_ENABLE_MAX_THREADS "Enable maximum machine threads" OFF)

# Function to ensure valid thread count
function(validate_and_set_thread_count)
    include(ProcessorCount)
    ProcessorCount(NUM_CPU_CORES)
    
    # Ensure we have at least 1 thread when threading is enabled
    if(NUM_CPU_CORES EQUAL 0)
        message(WARNING "Could not determine hardware concurrency. Defaulting to 2 threads. Properly define STALKER_THREADING_NUM_THREADS.")
        set(STALKER_THREADING_NUM_THREADS 2 CACHE STRING "Thread count" FORCE)
        return()
    endif()
    
    # Handle MAX_THREADS override
    if(STALKER_THREADING_ENABLE_MAX_THREADS)
        set(STALKER_THREADING_NUM_THREADS ${NUM_CPU_CORES} CACHE STRING "" FORCE)
        return()
    endif()
    
    # Validate user-specified thread count
    if(${STALKER_THREADING_NUM_THREADS} EQUAL 0)
        # Auto-detect: use all available cores
        set(STALKER_THREADING_NUM_THREADS ${NUM_CPU_CORES} CACHE STRING "" FORCE)
        message(STATUS "Auto-detected hardware concurrency: ${NUM_CPU_CORES} threads")
    elseif(${STALKER_THREADING_NUM_THREADS} GREATER ${NUM_CPU_CORES})
        # Clamp to max available cores
        set(STALKER_THREADING_NUM_THREADS ${NUM_CPU_CORES} CACHE STRING "" FORCE)
        message(WARNING "STALKER_THREADING_NUM_THREADS exceeds hardware concurrency (${NUM_CPU_CORES}). "
                        "Clamping to maximum available threads: ${NUM_CPU_CORES}")
    elseif(${STALKER_THREADING_NUM_THREADS} LESS 1)
        # Force minimum 1 thread
        set(STALKER_THREADING_NUM_THREADS 2 CACHE STRING "" FORCE)
        message(WARNING "STALKER_THREADING_NUM_THREADS must be at least 2. Forcing to 2 threads.")
    endif()
endfunction()

if(STALKER_THREADING_ENABLE)
    # Set default thread count to 0 (auto-detect)
    if(NOT DEFINED STALKER_THREADING_NUM_THREADS)
        set(STALKER_THREADING_NUM_THREADS 0 CACHE STRING "Number of threads (0=auto-detect)" FORCE)
        message(STATUS "STALKER_THREADING_NUM_THREADS defaulting to 0 (auto-detect)")
    endif()
    
    # Validate thread count is a non-negative integer
    if(NOT STALKER_THREADING_NUM_THREADS MATCHES "^[0-9]+$")
        message(FATAL_ERROR "STALKER_THREADING_NUM_THREADS must be a non-negative integer")
    endif()
    
    # Ensure valid thread count
    validate_and_set_thread_count()
else()
    set(STALKER_THREADING_NUM_THREADS 0 CACHE STRING "Threading disabled" FORCE)
    message(STATUS "STALKER_THREADING_ENABLE=OFF: Forcing STALKER_THREADING_NUM_THREADS=0")
endif()

option(STALKER_THREADING_ENABLE_STDTHREAD      "Use std::thread for multithreading" OFF)
option(STALKER_THREADING_ENABLE_PTHREAD        "Use pthreads for multithreading (Linux/POSIX only)" ON)
option(STALKER_THREADING_ENABLE_SMT "Enable hyperthreading (only valid with pthreads on Linux)" ON)

if(STALKER_THREADING_ENABLE)

    if(NOT DEFINED STALKER_PLATFORM)
        message(FATAL_ERROR "STALKER_PLATFORM must be defined.")
    endif()

    # On Windows: pthread is always OFF, std::thread is ON, hyperthreading is always OFF
    if(STALKER_PLATFORM STREQUAL "WINDOWS")
        if(STALKER_THREADING_ENABLE_PTHREAD OR NOT STALKER_THREADING_ENABLE_STDTHREAD OR STALKER_THREADING_ENABLE_SMT)
            set(STALKER_THREADING_ENABLE_PTHREAD        OFF CACHE BOOL "" FORCE)
            set(STALKER_THREADING_ENABLE_STDTHREAD      ON  CACHE BOOL "" FORCE)
            set(STALKER_THREADING_ENABLE_SMT OFF CACHE BOOL "" FORCE)
            message(STATUS "Windows detected: Forcing STALKER_THREADING_ENABLE_PTHREAD=OFF, STALKER_THREADING_ENABLE_STDTHREAD=ON, STALKER_THREADING_ENABLE_SMT=OFF.")
        endif()
    endif()

    # Only one backend can be enabled
    if(STALKER_THREADING_ENABLE_PTHREAD AND STALKER_THREADING_ENABLE_STDTHREAD)
        message(FATAL_ERROR "Cannot enable both pthread and std::thread. Choose one backend.")
    endif()

    # If neither is enabled, select platform default
    if(NOT STALKER_THREADING_ENABLE_PTHREAD AND NOT STALKER_THREADING_ENABLE_STDTHREAD)
        if(STALKER_PLATFORM STREQUAL "WINDOWS")
            set(STALKER_THREADING_ENABLE_STDTHREAD ON CACHE BOOL "" FORCE)
            message(STATUS "No thread backend enabled, defaulting to std::thread (Windows).")
        else()
            set(STALKER_THREADING_ENABLE_PTHREAD    ON CACHE BOOL "" FORCE)
            set(STALKER_THREADING_ENABLE_STDTHREAD  OFF CACHE BOOL "" FORCE)
            set(STALKER_THREADING_ENABLE_SMT        OFF CACHE BOOL "" FORCE)
            message(STATUS "No thread backend enabled, defaulting to pthread (Linux/POSIX).")
        endif()
    endif()

    # If std::thread, force hyperthreading off
    if(STALKER_THREADING_ENABLE_STDTHREAD AND STALKER_THREADING_ENABLE_SMT)
        set(STALKER_THREADING_ENABLE_SMT OFF CACHE BOOL "" FORCE)
        message(STATUS "Hyperthreading is not supported with std::thread, forcing OFF.")
    endif()

else()
    set(STALKER_THREADING_ENABLE_PTHREAD        OFF CACHE BOOL "" FORCE)
    set(STALKER_THREADING_ENABLE_STDTHREAD      OFF CACHE BOOL "" FORCE)
    set(STALKER_THREADING_ENABLE_SMT            OFF CACHE BOOL "" FORCE)
endif()


################################################################################
# INTERFACE LIBRARY TARGET
################################################################################

add_library(StalkerCore INTERFACE)

# Public include path (for both build and install)
target_include_directories(StalkerCore INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

# Platform macros
if(STALKER_PLATFORM STREQUAL "WINDOWS")
    target_compile_definitions(StalkerCore INTERFACE STALKER_PLATFORM_WINDOWS)
elseif(STALKER_PLATFORM STREQUAL "LINUX")
    target_compile_definitions(StalkerCore INTERFACE STALKER_PLATFORM_LINUX)
else()
    message(FATAL_ERROR "Unsupported platform: ${STALKER_PLATFORM}. Only WINDOWS and LINUX are supported.")
endif()

# Bitness macros
if(STALKER_PLATFORM_BITS STREQUAL "64")
    target_compile_definitions(StalkerCore INTERFACE STALKER_64BIT)
elseif(STALKER_PLATFORM_BITS STREQUAL "32")
    target_compile_definitions(StalkerCore INTERFACE STALKER_32BIT)
endif()

target_compile_definitions(StalkerCore INTERFACE
    STALKER_PLATFORM_CACHE_LINE_SIZE=${STALKER_PLATFORM_CACHE_LINE_SIZE}
    STALKER_ALIGNMENT=${STALKER_ALIGNMENT}
    STALKER_SIMD_ENABLE=$<BOOL:${STALKER_SIMD_ENABLE}>
    STALKER_UNROLL_ENABLE=$<BOOL:${STALKER_UNROLL_ENABLE}>
    STALKER_UNROLL_FACTOR=${STALKER_UNROLL_FACTOR}
    IS_POSIX=$<BOOL:${IS_POSIX}>
    STALKER_THREADING_ENABLE=$<BOOL:${STALKER_THREADING_ENABLE}>
    STALKER_THREADING_NUM_THREADS=${STALKER_THREADING_NUM_THREADS}
    STALKER_THREADING_ENABLE_PTHREAD=$<BOOL:${STALKER_THREADING_ENABLE_PTHREAD}>
    STALKER_THREADING_ENABLE_SMT=$<BOOL:${STALKER_THREADING_ENABLE_SMT}>
    STALKER_THREADING_ENABLE_STDTHREAD=$<BOOL:${STALKER_THREADING_ENABLE_STDTHREAD}>
)


# SIMD instruction set macros and flags
if(STALKER_SIMD_ENABLE)
    if(STALKER_SIMD_INSTRUCTION_SET STREQUAL "avx2")
        target_compile_definitions(StalkerCore INTERFACE STALKER_SIMD_INSTRUCTION_SET_AVX2)
        target_compile_options(StalkerCore INTERFACE
            $<$<CXX_COMPILER_ID:GNU,Clang>:-mavx2 -mfma>
            $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
        )
    elseif(STALKER_SIMD_INSTRUCTION_SET STREQUAL "avx512")
        target_compile_definitions(StalkerCore INTERFACE STALKER_SIMD_INSTRUCTION_SET_AVX512)
        target_compile_options(StalkerCore INTERFACE
            $<$<CXX_COMPILER_ID:GNU,Clang>:-mavx512f -mavx512bw -mfma>
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

    if(STALKER_SIMD_STORE_POLICY STREQUAL "stream")
        target_compile_definitions(StalkerCore INTERFACE STALKER_SIMD_STORE_STREAM)
    elseif(STALKER_SIMD_STORE_POLICY STREQUAL "cache")
        target_compile_definitions(StalkerCore INTERFACE STALKER_SIMD_STORE_CACHE)
    else()
        message(FATAL_ERROR "Invalid STALKER_SIMD_STORE_POLICY: ${STALKER_SIMD_STORE_POLICY}. Must be 'stream' or 'cache'.")
    endif()
endif()

# Hiph Performance flags
option(STALKER_ALL_GAS_NO_BRAKES "Enable maximum performance compiler flags" OFF)

if(STALKER_ALL_GAS_NO_BRAKES)
    target_compile_options(StalkerCore INTERFACE
        # GCC and Clang only, for release/max perf builds
        $<$<CXX_COMPILER_ID:GNU,Clang>:
            -O3                 # Maximum optimization
            -ffast-math         # Aggressive FP optimization (can break IEEE edge cases, but is FAST)
            -funroll-loops      # Always unroll loops
            -march=native       # Target all extensions for the local machine (AVX2, AVX512, etc)
            -flto               # Link Time Optimization
            -fomit-frame-pointer # Save a register, minor perf
            -falign-functions=32 # Better function alignment for branch prediction on modern CPUs
            -falign-loops=32    # Align loops to cache lines (optional, but can help)
            -fno-math-errno     # Don't set errno for FP math (makes math ops faster)
            -DNDEBUG            # Remove asserts (standard for Release builds)
        >
        # MSVC (Windows)
        $<$<CXX_COMPILER_ID:MSVC>:
            /Ox         # Full optimization
            /fp:fast    # Fast floating-point math
            /GL         # Link Time Optimization
            /Ot         # Favor speed
            /DNDEBUG    # Remove asserts
            >
        )
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

message(STATUS "")
message(STATUS "=========================================")
message(STATUS "========== STALKER BUILD CONFIG ==========") 
message(STATUS "==========================================")
message(STATUS "")
message(STATUS "STALKER_PLATFORM: ${STALKER_PLATFORM}")
message(STATUS "STALKER_PLATFORM_BITS: ${STALKER_PLATFORM_BITS}")
message(STATUS "STALKER_PLATFORM_CACHE_LINE_SIZE: ${STALKER_PLATFORM_CACHE_LINE_SIZE}")
message(STATUS "STALKER_SIMD_ENABLE: ${STALKER_SIMD_ENABLE}")
message(STATUS "STALKER_SIMD_INSTRUCTION_SET: ${STALKER_SIMD_INSTRUCTION_SET}")
message(STATUS "STALKER_UNROLL_ENABLE: ${STALKER_UNROLL_ENABLE}")
message(STATUS "STALKER_UNROLL_FACTOR: ${STALKER_UNROLL_FACTOR}")
message(STATUS "IS_POSIX: ${IS_POSIX}")
message(STATUS "STALKER_THREADING_ENABLE: ${STALKER_THREADING_ENABLE}")
message(STATUS "STALKER_THREADING_NUM_THREADS: ${STALKER_THREADING_NUM_THREADS}")
message(STATUS "STALKER_THREADING_ENABLE_PTHREAD: ${STALKER_THREADING_ENABLE_PTHREAD}")
message(STATUS "STALKER_THREADING_ENABLE_STDTHREAD: ${STALKER_THREADING_ENABLE_STDTHREAD}")
message(STATUS "STALKER_THREADING_ENABLE_SMT: ${STALKER_THREADING_ENABLE_SMT}")
message(STATUS "=========================================")

get_cmake_property(_variableNames CACHE_VARIABLES)
foreach (_variableName IN LISTS _variableNames)
    if (_variableName MATCHES "^STALKER_")
        message(STATUS "${_variableName} = ${${_variableName}}")
    endif()
endforeach()

