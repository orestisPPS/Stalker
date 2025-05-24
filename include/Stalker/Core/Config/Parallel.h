#pragma once

/**
 * @file Parallel.h
 * @brief Configuration macros for enabling parallelism in the Stalker Core module.
 *
 * This header defines macros to control the use of internal threading and OpenMP
 * for parallelism within the Stalker project. It allows toggling between an in-house
 * thread pool and OpenMP, ensuring only one parallelism backend is active at a time.
 *
 * Macros:
 * - STALKER_INTERNAL_THREADING: Set to 1 if internal threading is enabled, otherwise 0.
 * - STALKER_PARALLELISM_ENABLED: Set to 1 if either internal threading or OpenMP is enabled, otherwise 0.
 *
 * Notes:
 * - If both internal threading and OpenMP are requested, internal threading takes precedence,
 *   and OpenMP is disabled with a warning.
 */
    // In-house thread pool
    #ifdef STALKER_ENABLE_INTERNAL_THREADING
        #define STALKER_INTERNAL_THREADING 1
        #if defined(STALKER_OPENMP)
            #warning "Internal threading overriding OpenMP"
            #undef STALKER_OPENMP
        #endif
    #else
        #define STALKER_INTERNAL_THREADING 0
    #endif

    #if defined(STALKER_OPENMP) || defined(STALKER_INTERNAL_THREADING)
        #define STALKER_PARALLELISM_ENABLED 1
    #else
        #define STALKER_PARALLELISM_ENABLED 0
    #endif
