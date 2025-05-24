

/**
 * @file Alignment.h
 * @brief Defines macros for cache line size and aligned memory allocation.
 *
 * This header provides platform-specific definitions for cache line size and
 * macros to assist with aligned memory allocation and deallocation.
 *
 * Macros:
 * - STALKER_CACHE_LINE_SIZE: Cache line size in bytes, determined by platform.
 *   - 16 bytes for ESP32 platforms.
 *   - 64 bytes for AArch64 and most x86/x64 and ARM CPUs.
 *   - 128 bytes for POWER8/POWER9.
 *
 * - STALKER_ALIGN(N): Specifies alignment for variables or structures.
 *   - Uses __declspec(align(N)) on MSVC.
 *   - Uses __attribute__((aligned(N))) on other compilers.
 *
 * - STALKER_ALIGNED_ALLOC(size, align): Allocates memory with specified alignment.
 *   - Uses _aligned_malloc on MSVC.
 *   - Uses aligned_alloc on other compilers.
 *
 * - STALKER_ALIGNED_FREE(ptr): Frees memory allocated with STALKER_ALIGNED_ALLOC.
 *   - Uses _aligned_free on MSVC.
 *   - Uses free on other compilers.
 */
#pragma once

#ifndef STALKER_CACHE_LINE_SIZE
    // ESP platform detection (using standard ESP-IDF define)
    #if defined(ESP_PLATFORM) || defined(STALKER_ESP32) || defined(CONFIG_IDF_TARGET_ESP32)
        // ESP32 cache line is 16 bytes
        #define STALKER_CACHE_LINE_SIZE 16
    #elif defined(__aarch64__)
        #define STALKER_CACHE_LINE_SIZE 64
    #elif defined(__POWER8__) || defined(__POWER9__)
        #define STALKER_CACHE_LINE_SIZE 128
    #else
        // Default for x86/x64, and most ARM CPUs
        #define STALKER_CACHE_LINE_SIZE 64
    #endif
#endif

#if defined(_MSC_VER)
    #define STALKER_ALIGN(N) __declspec(align(N))
    #define STALKER_ALIGNED_ALLOC(size, align) _aligned_malloc(size, align)
    #define STALKER_ALIGNED_FREE(ptr) _aligned_free(ptr)
#else
    #define STALKER_ALIGN(N) __attribute__((aligned(N)))
    #define STALKER_ALIGNED_ALLOC(size, align) aligned_alloc(align, size)
    #define STALKER_ALIGNED_FREE(ptr) free(ptr)
#endif