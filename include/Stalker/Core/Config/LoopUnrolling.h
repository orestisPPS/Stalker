#pragma once

/**
 * @file LoopUnrolling.h
 * @brief Provides macros for controlling loop unrolling optimizations across different compilers.
 *
 * This header defines macros to assist with loop unrolling pragmas in a cross-compiler manner.
 * - The default unroll factor can be set via the `STALKER_UNROLL_FACTOR` macro (defaults to 4).
 * - The `STALKER_UNROLL(N)` macro applies a compiler-specific pragma to unroll loops by N iterations.
 * - The `STALKER_UNROLL_DEFAULT` macro applies the unroll pragma using the default unroll factor.
 * - If the compiler does not support unroll pragmas, these macros expand to nothing.
 *
 * Supported compilers:
 *   - Clang: Uses `clang loop unroll_count`.
 *   - GCC (version 8 and above): Uses `GCC unroll`.
 *   - Other compilers: Macros expand to empty.
 *
 * Usage example:
 * @code
 * STALKER_UNROLL(8)
 * for (int i = 0; i < N; ++i) {
 *     // loop body
 * }
 * @endcode
 */
// Default unroll factor
#ifndef STALKER_UNROLL_FACTOR
    #define STALKER_UNROLL_FACTOR 4
#endif

// Helper macro for string expansion
#define STRINCLY(x) _STRINCLY(x)
#define _STRINCLY(x) #x

#if defined(__clang__)
    #define STALKER_UNROLL(N) _Pragma(STRINCLY("clang loop unroll_count(" #N ")"))
    #define STALKER_UNROLL_DEFAULT _Pragma("clang loop unroll_count(" STRINCLY(STALKER_UNROLL_FACTOR) ")")
#elif defined(__GNUC__) && (__GNUC__ >= 8)
    #define STALKER_UNROLL(N) _Pragma(STRINCLY("GCC unroll " #N))
    #define STALKER_UNROLL_DEFAULT _Pragma("GCC unroll " STRINCLY(STALKER_UNROLL_FACTOR))
#else
    #define STALKER_UNROLL(N)
    #define STALKER_UNROLL_DEFAULT
#endif
