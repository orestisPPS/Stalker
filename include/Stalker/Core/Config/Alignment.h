/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

/**
 * @file Alignment.h
 * @brief Compile-time alignment configuration and aligned memory macros for Stalker Core.
 * *
 * @section info Alignment Info
 * **Alignment** in computer architecture refers to arranging data in memory at addresses
 *  that are multiples of a given power of two (the alignment boundary).  
 *
 * @par Scientific Background
 * On virtually all CPU architectures, most primitive data types (integers, floats, pointers, vectors, etc) have strict alignment requirements:  
 * - **Alignment** is the number of bytes between permitted starting addresses for a given data type or structure.  
 * - A type with alignment N must be located at a memory address that is a multiple of N (e.g., 64-byte alignment means addresses divisible by 64).  
 * - Misaligned access can lead to significant performance penalties (extra memory cycles, partial loads, microcode assists), and on some platforms, *hardware exceptions* or outright faults.
 * - High-performance computing (HPC), vectorized/SIMD code, and cache-optimized algorithms depend on correct alignment to ensure fast, predictable access and avoid cache-line splits and false sharing.
 * - Alignment to cache line (typically 64 bytes on modern CPUs) minimizes cache conflicts, supports SIMD/vector loads, and guarantees safe use of hardware intrinsics.
 * - Proper alignment is required for `aligned_alloc`, for hardware SIMD instructions (e.g., AVX/AVX512), and to prevent undefined behavior in strict-aliasing scenarios.
 * - For dynamic allocations, aligned allocators guarantee pointer returns are suitable for all standard loads/stores and SIMD types.
 *
 * @section overview Overview
 * This header exposes the alignment policy for Stalker Core at compile time,
 * including the required alignment macro, the configured alignment value,
 * and portable macros for aligned variable and memory allocation.
 *
 * All values are defined by CMake at configure time and injected via macros.
 * By default, the alignment is set to the platform's cache line size,
 * but may be overridden by the user at configure time.
 *
 * @section config Preprocessor Configuration
 * - @b STALKER_ALIGNMENT (integer, always defined)
 *     The alignment (in bytes) to use for all core memory allocations and structures.
 *     Guaranteed to be a positive power of 2. Default: platform cache line size.
 *
 * @section macros Macros
 * - @b STALKER_ALIGN(N)
 *     Decorator for specifying alignment of a type or variable to N bytes.
 *     Usage: `STALKER_ALIGN(64) double arr[8];`
 * - @b STALKER_ALIGNED_ALLOC(size, align)
 *     Allocates `size` bytes aligned to `align`. Portable between MSVC and POSIX.
 * - @b STALKER_ALIGNED_FREE(ptr)
 *     Deallocates memory allocated with STALKER_ALIGNED_ALLOC.
 *
 * @section utilities Utilities
 * - @c inline constexpr int DefaultAlignment()
 *     Returns the compile-time default alignment (in bytes).
 * - @c inline constexpr bool IsValidAlignment(int)
 *     Returns true if the provided alignment is a positive power of 2.
 *
 * @section usage Usage Example
 * @code
 * using namespace Stalker::Core::Config;
 * STALKER_ALIGN(DefaultAlignment()) float cache_block[DefaultAlignment()];
 * void* ptr = STALKER_ALIGNED_ALLOC(4096, DefaultAlignment());
 * // ...
 * STALKER_ALIGNED_FREE(ptr);
 * @endcode
 *
 * @section references References
 * - https://en.cppreference.com/w/c/memory/aligned_alloc
 * - https://en.cppreference.com/w/cpp/compiler_support/17
 */

#ifndef STALKER_ALIGNMENT
    #error "STALKER_ALIGNMENT must be set by CMake!"
#endif

#include <cstddef>
#include <cstdlib>

#if defined(_MSC_VER)
    #define STALKER_ALIGN(N) __declspec(align(N))
    #define STALKER_ALIGNED_ALLOC(size, align) _aligned_malloc(size, align)
    #define STALKER_ALIGNED_FREE(ptr) _aligned_free(ptr)
#else
    #define STALKER_ALIGN(N) __attribute__((aligned(N)))
    #define STALKER_ALIGNED_ALLOC(size, align) aligned_alloc(align, size)
    #define STALKER_ALIGNED_FREE(ptr) free(ptr)
#endif

namespace Stalker::Core::Config {

    /**
     * @brief Returns the compile-time default alignment in bytes.
     * @return Alignment value (power of 2), e.g., 64.
     */
    inline constexpr size_t DefaultAlignment() {
        return STALKER_ALIGNMENT;
    }

    /**
     * @brief Checks if the given alignment value is a valid positive power of 2.
     * @param n Alignment value to check.
     * @return true if valid (power of 2 and > 0), false otherwise.
     */
    inline constexpr bool IsValidAlignment(int n) {
        return (n > 0) && ((n & (n - 1)) == 0);
    }

    static_assert(IsValidAlignment(DefaultAlignment()), "STALKER_ALIGNMENT must be a positive power of 2.");
} // namespace Stalker::Core::Config

