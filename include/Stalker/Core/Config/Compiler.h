#pragma once

/**
 * @file Compiler.h
 * @brief Compiler detection and force-inline macro for Stalker Core.
 *
 * @section overview Overview
 * A **compiler** is a program that translates source code written in a high-level language (such as C++ or C) into machine code
 * or intermediate representations executable by a CPU or virtual machine. Modern compilers (like GCC, Clang, and MSVC) perform complex
 * optimizations, code generation, and architecture-specific tuning, often including heuristics for function inlining, vectorization,
 * and memory layout.
 *  * **Force inlining** tells the compiler to embed the full function body at every call site, bypassing normal inlining heuristics.
 * - Inlining eliminates call/return overhead, enables constant propagation, and improves autovectorization—*at the cost of larger binary size and possibly worse instruction cache locality*.
 * - Overuse can backfire: use only for performance-critical, tiny functions.
 * - `STALKER_FORCE_INLINE` maps to the strongest compiler-supported always-inline directive for each toolchain.
 
 *
 * @section macros Macros
 * - @b STALKER_FORCE_INLINE
 *   - Expands to the strongest force-inline directive for the detected compiler.
 *   - MSVC: `__forceinline`
 *   - Clang: `__attribute__((always_inline)) inline`
 *   - GCC: `__attribute__((always_inline)) inline`
 *   - Unknown compiler: falls back to `inline` (no force-inlining).
 * 
 * @section config Preprocessor Configuration
 * - @b STALKER_MSVC: Defined as 1 if using Microsoft Visual C++.
 * - @b STALKER_CLANG: Defined as 1 if using Clang.
 * - @b STALKER_GCC: Defined as 1 if using GCC (and not Clang).
 * - @b STALKER_UNKNOWN_COMPILER: Defined if none of the above matches.
 * - @b STALKER_FORCE_INLINE: Expands to the strongest force-inline for the detected compiler.
 *
 * @section references References
 * - https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes
 * - https://learn.microsoft.com/en-us/cpp/cpp/inline-functions-cpp
 * - https://clang.llvm.org/docs/AttributeReference.html#always-inline
 */

#if defined(_MSC_VER)
    #define STALKER_MSVC 1
    #define STALKER_FORCE_INLINE __forceinline
#elif defined(__clang__)
    #define STALKER_CLANG 1
    #define STALKER_FORCE_INLINE __attribute__((always_inline)) inline
#elif defined(__GNUC__)
    #define STALKER_GCC 1
    #define STALKER_FORCE_INLINE __attribute__((always_inline)) inline
#else
    #warning "STALKER: Unknown compiler—force-inline falls back to 'inline'. Some features may be limited."
    #define STALKER_UNKNOWN_COMPILER 1
    #define STALKER_FORCE_INLINE inline
#endif
