cmake_minimum_required(VERSION 3.18)

# Centralized configurable options for Stalker (header-only)

# Platform / bitness / cache line size are detected (non-overridable) in DetectPlatform.cmake.
# Provide read-only cache entries for GUI visibility; detection will overwrite these.
set(STALKER_PLATFORM "" CACHE STRING "Detected platform (WINDOWS, LINUX or ESP32). Read-only." FORCE)
set(STALKER_PLATFORM_BITS "" CACHE STRING "Detected architecture bitness (64 or 32). Read-only." FORCE)
set(STALKER_PLATFORM_CACHE_LINE_SIZE "" CACHE STRING "Detected hardware cache line size in bytes. Read-only." FORCE)

# Alignment (leave empty -> auto default to cache line size via ConfigureAlignment)
set(STALKER_ALIGNMENT "" CACHE STRING "Alignment (bytes) for allocations. Empty -> auto to cache line size." FORCE)

# SIMD options
option(STALKER_SIMD_ENABLE "Enable compile-time SIMD vectorization paths." ON)
set(STALKER_SIMD_INSTRUCTION_SET "auto" CACHE STRING "Requested default SIMD ISA: auto, avx2, avx512, esp-dsp, none (auto prefers AVX2)." FORCE)
set_property(CACHE STALKER_SIMD_INSTRUCTION_SET PROPERTY STRINGS auto avx2 avx512 esp-dsp none)
set(STALKER_SIMD_STORE_POLICY "stream" CACHE STRING "SIMD store policy: stream (non-temporal) or cache (temporal)." FORCE)
set_property(CACHE STALKER_SIMD_STORE_POLICY PROPERTY STRINGS stream cache)

# Prefetch
set(STALKER_SIMD_PREFETCH_LINES 1 CACHE STRING "Number of cache lines to prefetch ahead (0 to disable)." FORCE)
set(STALKER_SIMD_PREFETCH_HINT "HintT0" CACHE STRING "Prefetch locality hint (HintT0, HintT1, HintT2, HintNTA)." FORCE)
set_property(CACHE STALKER_SIMD_PREFETCH_HINT PROPERTY STRINGS HintT0 HintT1 HintT2 HintNTA)

# Capability result (populated by detection;)
set(STALKER_SIMD_AVX2_OK OFF CACHE BOOL "Detected AVX2 capability (compiler + intrinsic). Read-only." FORCE)
set(STALKER_SIMD_AVX512_OK OFF CACHE BOOL "Detected AVX512 capability (compiler + intrinsic). Read-only." FORCE)

# Unroll
set(STALKER_UNROLL_FACTOR 1 CACHE STRING "Loop unroll factor for meta/classic execution (power-of-two recommended)." FORCE)

# Threading options
option(STALKER_THREADING_ENABLE "Enable parallel (multi-threaded) execution of operations." ON)
option(STALKER_THREADING_MAX_THREADS_ENABLE "Use all available hardware threads (overrides manual count)." OFF)
if(NOT DEFINED STALKER_THREADING_NUM_THREADS)
	set(STALKER_THREADING_NUM_THREADS 0 CACHE STRING "Thread count; 0 auto-detects logical CPUs (respecting SMT setting).")
endif()
option(STALKER_THREADING_STD_ENABLE "Use std::thread backend (portable)." ON)
option(STALKER_THREADING_POSIX_ENABLE "Use pthread backend (Linux only) for affinity control." OFF)
option(STALKER_THREADING_POSIX_SMT_ENABLE "Enable SMT (hyperthread siblings) with pthread backend." OFF)

# Unified build profile selector
# STALKER_BUILD_PROFILE: debug | release | perf | relwithdebinfo | custom
#   debug          -> -O0 -g
#   release        -> -O3 -DNDEBUG
#   perf           -> release base + aggressive tuning (fast-math, march=native, unroll, LTO, etc.)
#   relwithdebinfo -> -O2 -g -DNDEBUG
#   custom         -> user-supplied flags (STALKER_BUILD_CUSTOM_FLAGS)
if(NOT DEFINED STALKER_BUILD_PROFILE OR STALKER_BUILD_PROFILE STREQUAL "")
	set(STALKER_BUILD_PROFILE "release" CACHE STRING "Build profile: debug|release|perf|relwithdebinfo|custom")
endif()
set_property(CACHE STALKER_BUILD_PROFILE PROPERTY STRINGS debug release perf relwithdebinfo custom)
if(NOT DEFINED STALKER_BUILD_CUSTOM_FLAGS)
	set(STALKER_BUILD_CUSTOM_FLAGS "" CACHE STRING "Custom build flags (semicolon list) when STALKER_BUILD_PROFILE=custom.")
endif()

# Module selection: empty = all modules; else a semicolon list from {Core,Memory,Mathematics,Threading,Utility}
set(STALKER_MODULES "" CACHE STRING "Semicolon list of modules to build (empty=all). Core;Memory;Utility;Containers;Mathematics;Threading" )