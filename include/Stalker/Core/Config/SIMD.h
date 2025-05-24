#pragma once

// SIMD detection

/**
 * @file SIMD.h
 * @brief SIMD configuration macros for the Stalker::Core namespace.
 *
 * This header defines macros to enable and configure SIMD (Single Instruction, Multiple Data)
 * support within the Stalker::Core namespace. The configuration is controlled via preprocessor
 * definitions:
 *
 * - STALKER_ENABLE_SIMD: If defined, enables SIMD support (STALKER_SIMD set to 1), otherwise disables it (set to 0).
 * - STALKER_SIMD_INSTRUCTION_SET: If defined, specifies the SIMD instruction set to use.
 *   Supported values:
 *     - avx512: Sets STALKER_SIMD_LEVEL to 512.
 *     - avx2:   Sets STALKER_SIMD_LEVEL to 256.
 *     - esp-dsp: Sets STALKER_SIMD_LEVEL to 128.
 *   If an invalid value is provided, a preprocessor error is triggered.
 * - If STALKER_SIMD_INSTRUCTION_SET is not defined, the SIMD level is determined by compiler/platform macros:
 *     - __AVX512F__: Sets STALKER_SIMD_LEVEL to 512.
 *     - __AVX2__:    Sets STALKER_SIMD_LEVEL to 256.
 *     - STALKER_ESP32S3: Sets STALKER_SIMD_LEVEL to 128.
 *     - Otherwise, STALKER_SIMD_LEVEL is set to 0.
 *
 * Usage:
 *   Use STALKER_SIMD and STALKER_SIMD_LEVEL macros to conditionally compile SIMD-optimized code.
 */
#pragma once

#ifdef STALKER_ENABLE_SIMD
    #define STALKER_SIMD 1
#else
    #define STALKER_SIMD 0
#endif

#if STALKER_SIMD

    #if defined(STALKER_SIMD_INSTRUCTION_SET_AVX512)
        #define STALKER_SIMD_LEVEL 512
    #elif defined(STALKER_SIMD_INSTRUCTION_SET_AVX2)
        #define STALKER_SIMD_LEVEL 256
    #elif defined(STALKER_SIMD_INSTRUCTION_SET_ESP_DSP)
        #define STALKER_SIMD_LEVEL 64
    #elif defined(STALKER_SIMD_INSTRUCTION_SET_NONE)
        #define STALKER_SIMD_LEVEL 0
    #else
        // Auto-detect from compiler flags if possible
        #if defined(__AVX512F__)
            #define STALKER_SIMD_LEVEL 512
        #elif defined(__AVX2__)
            #define STALKER_SIMD_LEVEL 256
        #elif defined(STALKER_ESP32S3)
            #define STALKER_SIMD_LEVEL 128
        #else
            #define STALKER_SIMD_LEVEL 0
        #endif
    #endif

#else
    #define STALKER_SIMD_LEVEL 0
#endif

