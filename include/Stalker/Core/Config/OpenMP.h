#pragma once

/**
 * @file OpenMP.h
 * @brief OpenMP configuration header for the Stalker::Core namespace.
 *
 * This header conditionally enables OpenMP support based on the presence of the
 * _OPENMP macro and the STALKER_ENABLE_OPENMP definition. If both are defined,
 * the STALKER_OPENMP macro is set to 1 and <omp.h> is included to enable
 * OpenMP parallelization features. Otherwise, STALKER_OPENMP is set to 0,
 * disabling OpenMP support.
 *
 * Usage:
 * - Check STALKER_OPENMP to determine if OpenMP features are available.
 * - Include this header in modules that require conditional OpenMP support.
 */
#if defined(_OPENMP) && defined(STALKER_ENABLE_OPENMP)
    #define STALKER_OPENMP 1
    #include <omp.h>
#else
    #define STALKER_OPENMP 0
#endif