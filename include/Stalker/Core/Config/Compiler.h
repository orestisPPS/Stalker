
#pragma once

/**
 * @file Compiler.h
 * @brief Compiler detection and force inline macro definitions for the Stalker::Core namespace.
 *
 * This header defines macros to detect the compiler being used (MSVC, Clang, GCC, or unknown)
 * and sets the appropriate force inline directive for each supported compiler.
 *
 * Macros:
 * - STALKER_MSVC: Defined if compiling with Microsoft Visual C++.
 * - STALKER_CLANG: Defined if compiling with Clang.
 * - STALKER_GCC: Defined if compiling with GCC.
 * - STALKER_UNKNOWN_COMPILER: Defined if the compiler is not recognized.
 * - STALKER_FORCE_INLINE: Expands to the compiler-specific force inline keyword or attribute.
 *
 * A warning is issued if the compiler is not recognized, and a fallback to 'inline' is provided.
 */
#if defined(_MSC_VER)
    #define STALKER_MSVC 1
    #define STALKER_FORCE_INLINE __forceinline
#elif defined(__clang__)
    #define STALKER_CLANG 1
    #define STALKER_FORCE_INLINE __attribute__((always_inline))
#elif defined(__GNUC__)
    #define STALKER_GCC 1
    #define STALKER_FORCE_INLINE __attribute__((always_inline))
#else
    #warning "Unsupported compiler - some features may be limited"
    #define STALKER_UNKNOWN_COMPILER 1
    #define STALKER_FORCE_INLINE inline
#endif
