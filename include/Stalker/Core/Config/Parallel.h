#pragma once

#include <Stalker/Core/Config/Platform.h>

/**
 * @file Parallel.h
 * @brief Compile-time threading configuration and backend selection for Stalker Core.
 *
 * @section overview Overview
 * This header defines compile-time configuration for parallel execution in Stalker Core,
 * including thread backend selection and configuration.
 * Threading in modern high-performance code is critical for leveraging multicore processors,
 * improving throughput, and minimizing idle CPU cycles. Multithreading is an execution model
 * that allows a single process to run multiple independent instruction streams (threads) concurrently,
 * sharing resources to leverage multi-core processors for parallel computation, thereby increasing
 * performance and throughput. Correct use of threading can significantly improve performance for
 * compute-bound workloads, but requires careful control to avoid race conditions and oversubscription.
 * Simultaneous Multithreading (SMT) allows multiple threads to run on a single physical core, sharing the
 * core's caches and execution units.
 *
 * @section threading_model Threading Model
 * Stalker Core provides two threading backends with configurable selection at build time:
 * 
 * - **pthread** (POSIX threads):
 *   - Low-level thread primitives available on Linux and other POSIX-compliant systems.
 *   - Supports optional CPU affinity and hyperthreading (SMT) control.
 * 
 * - **std::thread** (C++ standard threads):
 *   - Portable high level thread abstraction available on all major platforms.
 *   - Simpler but with no control over CPU affinity and SMT.
 * 
 * @note Hyperthreading (SMT) is only supported with the pthread backend on Linux.
 *
 * Backend selection is enforced at compile time based on platform and build options:
 * - On Windows: Only std::thread is valid.
 * - On Linux/POSIX: pthread is default; std::thread can be selected explicitly.
 *
 * @section thread_count Thread Count Behavior
 * Thread count is configured via build system macros with the following rules:
 * - If threading is disabled, thread count is forcibly 0.
 * - If @c STALKER_THREADING_ENABLE_MAX_THREADS is set, maximum available threads will be defaulted.
 *
 * @section enumerations Enumerations
 * @enum ThreadType
 * Enumerates supported threading backends:
 *   - @c STDThread : C++ standard threads (std::thread).
 *   - @c PThread  : POSIX threads (pthreads).
 *   - @c None     : Threading disabled (single-threaded execution).
 * 
 * @section config Preprocessor Configuration
 * The following macros must be defined by the build system (CMake):
 * - @b STALKER_THREADING_ENABLE (0/1):
 *   - Enables or disables threading globally.
 * - @b STALKER_THREADING_NUM_THREADS:
 *   - Configured thread count.
 * - @b STALKER_THREADING_ENABLE_PTHREAD (0/1):
 *   - Use pthread backend (Linux/POSIX only).
 * - @b STALKER_THREADING_ENABLE_STDTHREAD (0/1):
 *   - Use std::thread backend.
 * - @b STALKER_THREADING_ENABLE_SMT (0/1):
 *   - Enables hyperthreading (SMT); only valid with pthread on Linux.
 * - @b STALKER_THREADING_ENABLE_MAX_THREADS (0/1):
 *   - Forces maximum hardware threads regardless of user-specified count.
 *
 * @section utilities Utilities
 * The namespace @c Stalker::Core::Config provides:
 * - @c constexpr bool isThreadingEnabled():
 *   - Returns true if threading is enabled.
 * - @c constexpr bool isStdThreadEnabled():
 *   - Returns true if std::thread backend is active.
 * - @c constexpr bool isPthreadEnabled():
 *   - Returns true if pthread backend is active.
 * - @c constexpr bool isSMTEnabled():
 *   - Returns true if Simultaneous Multithreading (SMT) is enabled.
 * - @c constexpr int DefaultNumThreads():
 *   - Returns configured thread count (0 = auto-detect).
 *
 * @section references References
 * - https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_create.html
 * - https://en.cppreference.com/w/cpp/thread/thread
 * - https://en.wikipedia.org/wiki/Simultaneous_multithreading
 */


#if STALKER_THREADING_ENABLE

    #if STALKER_THREADING_ENABLE_PTHREAD
        #ifndef STALKER_PLATFORM_LINUX
            #error "pthread backend is only supported when STALKER_PLATFORM_LINUX is defined.\n" \
                   "You better check yo self before you wreck yo self."
        #endif
    #endif

    #if !(STALKER_THREADING_ENABLE_PTHREAD || STALKER_THREADING_ENABLE_STDTHREAD)
        #error "STALKER_THREADING_ENABLE is ON but no backend selected.\n" \
               "You better check yo self before you wreck yo self."
    #endif

    #if (STALKER_THREADING_ENABLE_PTHREAD) && (STALKER_THREADING_ENABLE_STDTHREAD)
        #error "Both pthread and std::thread backends are enabled. This is invalid.\n" \
               "You better check yo self before you wreck yo self."
    #endif

    #if STALKER_THREADING_ENABLE_STDTHREAD && STALKER_THREADING_ENABLE_SMT
        #error "Hyperthreading (SMT) cannot be enabled with std::thread backend.\n" \
               "You better check yo self before you wreck yo self."
    #endif

    #if STALKER_THREADING_NUM_THREADS < 1
        #error "STALKER_THREADING_NUM_THREADS must be at least 1.\n" \
               "You better check yo self before you wreck yo self."
    #endif

#else

    #if STALKER_THREADING_ENABLE_PTHREAD || STALKER_THREADING_ENABLE_STDTHREAD || STALKER_THREADING_ENABLE_SMT
        #error "Threading is disabled but backend-specific macros are set.\n" \
               "You better check yo self before you wreck yo self."
    #endif

#endif

#if STALKER_THREADING_ENABLE
    #if STALKER_THREADING_ENABLE_PTHREAD
        #include <pthread.h>
    #elif STALKER_THREADING_ENABLE_STDTHREAD
        #include <thread>
    #else
        #error "Threading enabled but no valid backend detected."
    #endif
#endif

namespace Stalker::Core::Config {
    /**
     * @enum ThreadType
     * @brief Enumerates supported threading backends for Stalker Core.
     *
     * Stalker Core supports multiple threading backends for parallel execution. This
     * enumeration reflects the active backend selected at compile time based on platform
     * and build configuration. Only one backend is active at a time:
     *
     * - ThreadType::PThread : POSIX threads backend (pthread), available on Linux/POSIX systems.
     * - ThreadType::STDThread : C++ standard threading backend (std::thread), portable across platforms.
     * - ThreadType::None : Threading disabled, no parallel execution.
     *
     * The backend selection is controlled by build-time CMake macros and enforced by configuration headers.
     *
     * @section threadtype_backend Backend Characteristics
     * - **PThread**:
     *   - Low-level POSIX threading API.
     *   - Supports CPU affinity and hyperthreading (SMT) where available.
     *   - Recommended for Linux/POSIX targets requiring fine-grained control.
     *
     * - **STDThread**:
     *   - Portable C++ standard threading abstraction.
     *   - Available on all platforms.
     *   - Preferred for cross-platform portability with simplified thread management.
     *
     * - **None**:
     *   - Threading is disabled at compile time.
     *   - All execution is single-threaded.
     */
    enum class ThreadType {
        /**
         * @brief std::thread backend (portable C++ threads).
         *
         * Uses the standard C++ threading library for parallel execution.
         * Available on all major platforms.
         */
        STDThread,

        /**
         * @brief POSIX pthreads backend.
         *
         * Uses pthreads for thread creation and management.
         * Only valid on Linux/POSIX systems.
         */
        PThread,

        /**
         * @brief Threading disabled (single-threaded fallback).
         *
         * No multithreading is used; all code runs on a single thread.
         */
        None
    };

    /**
     * @brief Type alias for the active thread handle type based on the selected backend.
     * @details
     * - If pthread is enabled, this is an alias for `pthread_t`.
     * - If std::thread is enabled, this is an alias for `std::thread`.
     */
    #if STALKER_THREADING_ENABLE_PTHREAD
        using PlatformThread = pthread_t;
    #elif STALKER_THREADING_ENABLE_STDTHREAD
        using PlatformThread = std::thread;
    #endif


    /**
     * @brief Returns true if threading is enabled at compile time.
     * @return true if threading is globally enabled; false otherwise.
     *
     * If this returns false, the application runs entirely single-threaded.
     */
    constexpr inline bool isThreadingEnabled() {
        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Returns true if pthread backend is active.
     * @return true if pthread is the selected threading backend; false otherwise.
     */
    constexpr inline bool isPthreadEnabled() {
        #if defined(STALKER_THREADING_ENABLE_PTHREAD) && STALKER_THREADING_ENABLE_PTHREAD != 0
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Returns true if Simultaneous Multithreading (SMT) is enabled.
     * @return true if SMT is enabled (only valid with pthread backend); false otherwise.
    */
    constexpr inline bool isSMTEnabled() {
        #if defined(STALKER_THREADING_ENABLE_SMT) && STALKER_THREADING_ENABLE_SMT != 0
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Returns true if std::thread backend is active.
     * @return true if std::thread is the selected threading backend; false otherwise.
     */
    constexpr inline bool isStdThreadEnabled() {
        #if defined(STALKER_THREADING_ENABLE_STDTHREAD) && STALKER_THREADING_ENABLE_STDTHREAD != 0
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Returns the configured thread count.
     * @return Integer thread count (1 = single-thread fallback).
     *
     * If threading is disabled, this always returns 1. If threading is enabled,
     * and @c STALKER_THREADING_NUM_THREADS is defined, it returns that value.
     * Otherwise, it defaults to 2 threads.
     */
    constexpr inline int DefaultNumThreads() {
        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
            #if defined(STALKER_THREADING_NUM_THREADS)
                return STALKER_THREADING_NUM_THREADS;
            #else
                return 2;
            #endif
        #else
            return 1; // Single-threaded fallback
        #endif
    }

    constexpr inline ThreadType DefaultThreadType() {
        #if defined(STALKER_THREADING_ENABLE_PTHREAD) && STALKER_THREADING_ENABLE_PTHREAD != 0
            return ThreadType::PThread;
        #elif defined(STALKER_THREADING_ENABLE_STDTHREAD) && STALKER_THREADING_ENABLE_STDTHREAD != 0
            return ThreadType::STDThread;
        #else
            return ThreadType::None;
        #endif
    }

} // namespace Stalker::Core::Config