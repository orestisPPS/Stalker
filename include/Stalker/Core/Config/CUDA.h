#pragma once

/**
 * @file CUDA.h
 * @brief Configuration macros for CUDA support in the Stalker::Core namespace.
 *
 * This header defines macros to detect and configure CUDA compilation.
 * - If compiling with CUDA (`__CUDACC__` or `__CUDA_ARCH__` defined):
 *   - `STALKER_CUDA` and `STALKER_GPU` are set to 1.
 *   - If compiling for host (`__CUDA_ARCH__` not defined), `STALKER_CUDA_HOST` is set to 1.
 *   - If compiling for device (`__CUDA_ARCH__` defined), `STALKER_CUDA_DEVICE` is set to 1.
 * - Otherwise, `STALKER_CUDA` and `STALKER_GPU` are set to 0.
 */
// CUDA support
#if defined(__CUDACC__) || defined(__CUDA_ARCH__)
    #define STALKER_CUDA 1
    #define STALKER_GPU 1
    #ifndef __CUDA_ARCH__
        #define STALKER_CUDA_HOST 1
    #else
        #define STALKER_CUDA_DEVICE 1
    #endif
#else
    #define STALKER_CUDA 0
    #define STALKER_GPU 0
#endif
