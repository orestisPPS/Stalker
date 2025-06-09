#pragma once

/**
 * @file LoopUnrolling.h
 * @brief Compile-time loop unrolling configuration and utilities for Stalker Core.
 *
 * @section overview Overview
 * This header provides macros and constexpr utilities to control loop unrolling
 * via preprocessor configuration. Loop unrolling is a low-level optimization
 * technique that expands the loop body multiple times to reduce loop overhead,
 * increase instruction-level parallelism (ILP), and improve pipeline utilization
 * in numeric and vectorized code. Correct use of loop unrolling may yield
 * measurable performance benefits in tight computational loops, especially when
 * combined with SIMD, but it also increases binary size.
 *
 * @section config Preprocessor Configuration
 * The following macros must be defined by the build system (typically via CMake):
 * - @b STALKER_ENABLE_LOOP_UNROLL
 *   - Set to 1 to enable loop unrolling.
 *   - Set to 0 to disable all unrolling (macros expand to nothing).
 * - @b STALKER_UNROLL_FACTOR
 *   - Compile-time integer constant specifying the unroll count.
 *   - Must be a positive integer, typically a power of two (e.g. 4 or 8).
 *   - If unrolling is disabled, this is forcibly set to 1.
 *
 * @section macros Macros
 * - @b STALKER_UNROLL(N)
 *   - Expands to a compiler-specific pragma requesting loop unrolling by N iterations.
 *   - Clang:    \code #pragma clang loop unroll_count(N) \endcode
 *   - GCC ≥ 8:  \code #pragma GCC unroll N \endcode
 *   - Other compilers or if unrolling is disabled: expands to nothing.
 * - @b STALKER_UNROLL_DEFAULT
 *   - Expands to a pragma using the value of STALKER_UNROLL_FACTOR.
 *
 * @par Example: Requesting a specific unroll count (portable between Clang and GCC 8+)
 * @code
 * STALKER_UNROLL(8)
 * for (int i = 0; i < N; ++i) {
 *     // loop body
 * }
 * @endcode
 *
 * @par Example: Using the default compile-time unroll factor
 * @code
 * STALKER_UNROLL_DEFAULT
 * for (size_t j = 0; j < array_size; ++j) {
 *     a[j] += b[j] * c[j];
 * }
 * @endcode
 *
 * @section manual_unroll Classic Explicit Unrolling Example
 * For compilers/platforms where pragma-based unrolling is unavailable, or for
 * maximum control, loops can be unrolled manually as follows:
 * @code
 * constexpr int UnrollFactor = 4;
 * size_t limit = size - (size % UnrollFactor);
 * for (size_t i = 0; i < limit; i += UnrollFactor) {
 *     result[i    ] = a[i    ] + b[i    ];
 *     result[i + 1] = a[i + 1] + b[i + 1];
 *     result[i + 2] = a[i + 2] + b[i + 2];
 *     result[i + 3] = a[i + 3] + b[i + 3];
 * }
 * // Handle the tail if size is not a multiple of UnrollFactor
 * for (size_t i = limit; i < size; ++i)
 *     result[i] = a[i] + b[i];
 * @endcode
 *
 * @section utilities Utilities
 * The namespace @c Stalker::Core::Config provides:
 * - @c constexpr bool isLoopUnrollingEnabled()
 *   - Returns true if loop unrolling is enabled and the factor is greater than 1.
 * - @c constexpr int DefaultUnrollFactor()
 *   - Returns the default compile-time unroll factor.
 *
 * @section references References
 * - https://en.wikipedia.org/wiki/Loop_unrolling
 * - https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html
 */

#ifndef STALKER_UNROLL_FACTOR
    #error "STALKER_UNROLL_FACTOR must be set by CMake!"
#endif

#ifndef STALKER_ENABLE_LOOP_UNROLL
    #error "STALKER_ENABLE_LOOP_UNROLL must be set by CMake!"
#endif

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#if STALKER_ENABLE_LOOP_UNROLL // 1 or 0

    #if defined(__clang__)
        #define STALKER_UNROLL(N) _Pragma(STRINGIFY(clang loop unroll_count(N)))
        #define STALKER_UNROLL_DEFAULT _Pragma(STRINGIFY(clang loop unroll_count(STALKER_UNROLL_FACTOR)))
    #elif defined(__GNUC__) && (__GNUC__ >= 8)
        #define STALKER_UNROLL(N) _Pragma(STRINGIFY(GCC unroll N))
        #define STALKER_UNROLL_DEFAULT _Pragma(STRINGIFY(GCC unroll STALKER_UNROLL_FACTOR))
    #else
        #define STALKER_UNROLL(N)
        #define STALKER_UNROLL_DEFAULT
    #endif

#else
    #define STALKER_UNROLL(N)
    #define STALKER_UNROLL_DEFAULT
#endif

namespace Stalker::Core::Config {

    /**
     * @brief Returns true if loop unrolling is enabled and the factor > 1.
     * @return true if loop unrolling is enabled and factor > 1; false otherwise.
     *
     * If this returns false, unroll macros expand to nothing and there is no unrolling.
     */
    constexpr bool isLoopUnrollingEnabled() { return (STALKER_ENABLE_LOOP_UNROLL != 0) && (STALKER_UNROLL_FACTOR > 1); }

    /**
     * @brief Returns the compile-time unroll factor.
     * @return Integer unroll factor, set by CMake.
     * 
     * @note Defaults to 1 if loop unrolling is disabled.
     */
    constexpr size_t DefaultUnrollFactor() { return STALKER_UNROLL_FACTOR; }

    static_assert(STALKER_UNROLL_FACTOR >= 1, "STALKER_UNROLL_FACTOR must be >= 1");

} // namespace Stalker::Core::Config
