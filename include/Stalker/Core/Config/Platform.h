#pragma once

/**
 * @file Platform.h
 * @brief Platform and architecture detection/configuration for the Stalker Core library.
 *
 * @section overview Overview
 * This header exposes compile-time platform, OS, bitness, and cache line information.
 * All values are auto-detected by CMake during configuration. 
 * Supported platforms are Windows and Linux 64-bit and esp 32-bit.
 *
 * @section config Preprocessor Configuration
 * One of the following macros must be defined by CMake for platform and architecture detection:
 * - @b STALKER_PLATFORM_WINDOWS   Defined as 1 if targeting Windows, else undefined.
 * - @b STALKER_PLATFORM_LINUX     Defined as 1 if targeting Linux, else undefined.
 * - @b STALKER_64BIT              Defined as 1 if targeting a 64-bit platform, else undefined.
 * - @b STALKER_32BIT              Defined as 1 if targeting a 32-bit platform, else undefined.
 * - @b STALKER_PLATFORM_CACHE_LINE_SIZE    The native cache line size in bytes for this platform.
 *
 * These macros are guaranteed to be consistent with the configuration detected by CMake.
 *
 * @section enumerations Enumerations
 * @enum Platform
 *   Enumerates supported target platforms
 * -   Platform::Windows : Microsoft Windows OS.
 * -   Platform::Linux   : Linux OS (all distributions).
 * -   Platform::Unknown : Unknown or unsupported platform.
 * @enum Bitness
 *   Enumerates platform bitness
 * -   Bitness::x64 : 64-bit architecture (x86_64, amd64, aarch64).
 * -   Bitness::x86 : 32-bit architecture (i386, i686
 *
 * @section utilities Provided Utilities
 * - @c inline constexpr Platform CurrentPlatform()
 *     Returns the enumerator value corresponding to the current platform.
 * - @c inline constexpr Bitness CurrentBitness()
 *     Returns the enumerator value corresponding to the target bitness.
 * - @c inline constexpr bool IsWindows()
 *     Returns true if the build is targeting Windows.
 * - @c inline constexpr bool IsLinux()
 *     Returns true if the build is targeting Linux.
 * - @c inline constexpr bool Is64Bit()
 *     Returns true if building for a 64-bit architecture.
 * - @c inline constexpr bool Is32Bit()
 *     Returns true if building for a 32-bit architecture.
 * - @c inline constexpr int CacheLineSize()
 *     Returns the detected cache line size in bytes (64 or 32).
 *
 * @section references References
 * - https://en.wikipedia.org/wiki/X86-64
 * - https://en.wikipedia.org/wiki/Cache_line
 * - https://www.linux.org/
 */

#ifndef STALKER_PLATFORM_WINDOWS
    #ifndef STALKER_PLATFORM_LINUX
        #error "STALKER_PLATFORM_WINDOWS or STALKER_PLATFORM_LINUX must be set by CMake!"
    #endif
#endif

#if defined(STALKER_PLATFORM_WINDOWS) && STALKER_PLATFORM_WINDOWS
    #error "Either STALKER_PLATFORM_WINDOWS or STALKER_PLATFORM_LINUX must be defined by CMake!"
#endif

#ifndef STALKER_64BIT
    #ifndef STALKER_32BIT
        #error "STALKER_64BIT or STALKER_32BIT must be set by CMake!"
    #endif
#endif

#if defined(STALKER_64BIT) && STALKER_32BIT
    #error "Either STALKER_PLATFORM_WINDOWS or STALKER_PLATFORM_LINUX must be defined by CMake!"
#endif

#ifndef STALKER_PLATFORM_CACHE_LINE_SIZE
    #error "STALKER_PLATFORM_CACHE_LINE_SIZE must be set by CMake!"
#endif

namespace Stalker::Core::Config {

    /**
     * @enum Platform
     * @brief Supported operating system/platform for Stalker Core.
     *
     * Enumerates the target OS/platform. Used for low-level, OS-specific workarounds
     * and feature selection.
     */
    enum class Platform {
        /**
         * @brief Windows OS (Microsoft Windows).
         */
        Windows,
        /**
         * @brief Linux OS (all distributions).
         */
        Linux,
        /**
         * @brief Unknown or unsupported platform.
         */
        Unknown
    };

    /**
     * @enum Bitness
     * @brief Supported CPU bitness for Stalker Core.
     *
     * Enumerates the CPU bitness (word size) for the target architecture.
     * Used for memory, pointer, and alignment optimizations.
     */
    enum class Bitness {
        /**
         * @brief 64-bit architecture (x86_64, amd64, aarch64).
         */
        x64 = 64,
        /**
         * @brief 32-bit architecture (i386, i686, armv7, etc).
         */
        x86 = 32,
    };

    /**
     * @brief Returns the enumerator value for the current platform.
     * @return Platform value corresponding to this build.
     */
    inline constexpr Platform CurrentPlatform() {
    #if defined(STALKER_PLATFORM_WINDOWS) && STALKER_PLATFORM_WINDOWS
        return Platform::Windows;
    #elif defined(STALKER_PLATFORM_LINUX) && STALKER_PLATFORM_LINUX
        return Platform::Linux;
    #else
        return Platform::Unknown;
    #endif
    }

    /**
     * @brief Returns the enumerator value for the current bitness.
     * @return Bitness::x64 for 64-bit, Bitness::x86 for 32-bit.
     */
    inline constexpr Bitness CurrentBitness() {
    #if defined(STALKER_64BIT) && STALKER_64BIT
        return Bitness::x64;
    #elif defined(STALKER_32BIT) && STALKER_32BIT
        return Bitness::x86;
    #else
        static_assert(sizeof(void*) == 8 || sizeof(void*) == 4, "Platform must be 32 or 64 bit.");
        return Bitness::x64; // Default/fallback to 64.
    #endif
    }

    /**
     * @brief Returns true if this build targets Windows.
     * @return true if STALKER_PLATFORM_WINDOWS is defined.
     */
    inline constexpr bool IsWindows() {
    #if defined(STALKER_PLATFORM_WINDOWS) && STALKER_PLATFORM_WINDOWS
        return true;
    #else
        return false;
    #endif
    }

    /**
     * @brief Returns true if this build targets Linux.
     * @return true if STALKER_PLATFORM_LINUX is defined.
     */
    inline constexpr bool IsLinux() {
    #if defined(STALKER_PLATFORM_LINUX) && STALKER_PLATFORM_LINUX
        return true;
    #else
        return false;
    #endif
    }

    /**
     * @brief Returns true if this build targets a 64-bit CPU.
     * @return true if STALKER_64BIT is defined.
     */
    inline constexpr bool Is64Bit() {
    #if defined(STALKER_64BIT) && STALKER_64BIT
        return true;
    #else
        return false;
    #endif
    }

    /**
     * @brief Returns true if this build targets a 32-bit CPU.
     * @return true if STALKER_32BIT is defined.
     */
    inline constexpr bool Is32Bit() {
    #if defined(STALKER_32BIT) && STALKER_32BIT
        return true;
    #else
        return false;
    #endif
    }

    /**
     * @brief Returns the detected cache line size (in bytes) for the current platform.
     * @return 64 (default for 64-bit) or 32 (default for 32-bit platforms).
     */
    inline constexpr size_t CacheLineSize() {
        return STALKER_PLATFORM_CACHE_LINE_SIZE;
    }

} // namespace Stalker::Core::Config
