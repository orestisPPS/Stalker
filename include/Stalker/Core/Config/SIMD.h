/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

/**
 * @file SIMD.h
 * @brief Compile-time SIMD configuration and detection for the Stalker Core library.
 *
 * This header provides utilities to detect and configure support for
 * Single Instruction, Multiple Data (SIMD) instruction sets at compile time
 * based on CMake-defined macros. Supported instruction sets include AVX-512,
 * AVX2, ESP-DSP, and a scalar fallback.
 *
 * @section config Preprocessor Configuration
 * - @b STALKER_SIMD_ENABLE: Set to @c 1 to enable SIMD, @c 0 to disable (all SIMD paths excluded).
 * - Exactly one of the following must be defined when SIMD is enabled:
 *     - @b STALKER_SIMD_INSTRUCTION_SET_AVX512 Defined as 1 if AVX-512 support is enabled, else undefined.
 *     - @b STALKER_SIMD_INSTRUCTION_SET_AVX2 Defined as 1 if AVX2 support is enabled, else undefined.
 *     - @b STALKER_SIMD_INSTRUCTION_SET_ESP_DSP Defined as 1 if ESP-DSP support is enabled, else undefined.
 *     - @b STALKER_SIMD_INSTRUCTION_SET_NONE Defined as 1 if no SIMD support is enabled, else undefined.
 *      -@b STALKER_SIMD_SUPPORT_AVX2: Set to @c ON to enable AVX-512 support, @c OFF to disable.
 *      -@b STALKER_SIMD_SUPPORT_AVX2: Set to @c ON to enable AVX2 support, @c OFF to disable.
 * @section enumerations Enumerations
 * @enum T_SIMD
 * Enumerates supported SIMD instruction sets:
 *   - @c None    : No SIMD (scalar fallback).
 *   - @c AVX2    : 256-bit registers (4×64-bit doubles, 8×32-bit floats/ints, 16×16-bit shorts).
 *   - @c AVX512  : 512-bit registers (8×64-bit doubles, 16×32-bit floats/ints, 32×16-bit shorts).
 *   - @c ESP_DSP : 128-bit registers (4×32-bit floats/ints for ESP32-S3).
 *
 * @section utilities Provided Utilities
 * - @c inline constexpr bool IsSIMDEnabled()
 *     Returns true if any SIMD instruction set is enabled.
 * - @c inline constexpr bool IsAVX2Enabled()
 *     Returns true if AVX2 support is enabled.
 * - @c inline constexpr bool IsAVX512Enabled()
 *     Returns true if AVX-512 support is enabled.
 * - @c inline constexpr bool IsESPDSPEnabled()
 *     Returns true if ESP-DSP support is enabled.
 * - @c inline constexpr T_SIMD DefaultSIMDType()
 *     Returns the active T_SIMD (or @c None if disabled).
 * - @c inline constexpr const char* DefaultSIMDName()
 *     Returns the canonical name: `"avx512"`, `"avx2"`, `"esp-dsp"`, or `"none"`.
 * - @c inline constexpr size_t DefaultSIMDRegisterSize()
 *     Returns the register width in bits (512, 256, 128) or 0 if disabled.
 *
 * @section example Example Usage
 * @code{.cpp}
 * using namespace Stalker::Core::Config;
 * 
 * void foo() {
 *     if constexpr (IsAVX2Enabled()) {
 *         // Use AVX2 intrinsics here
 *     }
 * }
 * @endcode
 *
 * @section references References
 * - Intel Intrinsics Guide: https://www.intel.com/content/www/us/en/docs/intrinsics-guide
 * - ESP-DSP Library: https://docs.espressif.com/projects/esp-dsp
 */

#ifndef STALKER_SIMD_ENABLE
    #error "STALKER_SIMD_ENABLE must be set by CMake!"
#endif

#if defined(STALKER_SIMD_ENABLE) && STALKER_SIMD_ENABLE == 1
    #if !defined(STALKER_SIMD_INSTRUCTION_SET_AVX512)  && \
        !defined(STALKER_SIMD_INSTRUCTION_SET_AVX2)    && \
        !defined(STALKER_SIMD_INSTRUCTION_SET_ESP_DSP)
            #error "STALKER_SIMD_INSTRUCTION_SET must be set by CMake!"
    #endif
#endif


#if defined(STALKER_SIMD_ENABLE) && STALKER_SIMD_ENABLE == 1 && \
    (defined(STALKER_SIMD_AVX2_OK) || defined(STALKER_SIMD_AVX512_OK))
    #include <immintrin.h>
#endif

#include <cstddef>
namespace Stalker::Core::Config {

    /**
     * @enum T_SIMD
     * @brief Supported SIMD instruction sets for Stalker Core.
     *
     * Enumerates the hardware SIMD (Single Instruction, Multiple Data) instruction sets
     * supported by Stalker Core. Each enumerator value corresponds to the native register
     * width in bits for the relevant architecture.
     *
     * Supported instruction sets:
     *  - T_SIMD::None
     *  - T_SIMD::AVX2
     *  - T_SIMD::AVX512
     *  - T_SIMD::ESP_DSP
     */
    enum class T_SIMD {
        /**
         * @brief No SIMD support.
         *
         * SIMD operations are disabled or not available. All computation falls back to scalar code.
         */
        None = 0,

        /**
         * @brief AVX2: Advanced Vector Extensions 2.
         *
         * 256-bit SIMD registers. Typical lane width:
         *   - 4 × 64-bit double-precision floats (doubles)
         *   - 8 × 32-bit single-precision floats (floats)
         *   - 8 × 32-bit integers
         *   - 16 × 16-bit shorts
         *
         * Widely supported on modern Intel/AMD CPUs (Haswell and newer).
         */
        AVX2 = 256,

        /**
         * @brief AVX-512: Advanced Vector Extensions 512.
         *
         * 512-bit SIMD registers. Typical lane width:
         *   - 8 × 64-bit double-precision floats (doubles)
         *   - 16 × 32-bit single-precision floats (floats)
         *   - 16 × 32-bit integers
         *   - 32 × 16-bit shorts
         *
         * Present on high-end server/workstation Intel CPUs (Xeon/Ice Lake+, some AMD CPUs).
         */
        AVX512 = 512,

        /**
         * @brief ESP_DSP: ESP32-S3 DSP SIMD extension.
         *
         * 128-bit SIMD registers (microcontroller vector acceleration). Typical lane width:
         *   - 4 × 32-bit single-precision floats
         *   - 4 × 32-bit integers
         *
         * Supported on Espressif ESP32-S3 MCUs for embedded numeric DSP.
         */
        ESP_DSP = 128
    };


    /**
     * @enum T_SIMDStore
     * @brief Specifies the preferred memory store policy for SIMD output.
     *
     * SIMD store type determines how vectorized values are written to memory.
     * This allows fine-grained selection between *cache-friendly* stores (which
     * may benefit from temporal reuse and caching) and *streamed* (non-temporal)
     * stores (which minimize cache pollution for large bulk data writes).
     *
     * - T_SIMDStore::Cached   : Use normal (temporal) cacheable stores, i.e., _mm256_storeu*, _mm512_storeu*, etc.
     * - T_SIMDStore::Streamed : Use non-temporal (streaming) stores, i.e., _mm256_stream*, _mm512_stream*, etc.
     *
     * Streamed stores can improve performance for very large arrays that are not
     * reused soon (avoids polluting the cache). Cached stores are better for typical
     * workloads where data is read/written multiple times.
     */
    enum class T_SIMDStore {
        /**
         * @brief Normal cached stores (temporal).
         *
         * SIMD values are written to memory using regular cacheable store instructions,
         * allowing cache to retain and reuse written data.
         *
         * Example: _mm256_storeu_ps, _mm512_storeu_epi32, etc.
         */
        Cached,

        /**
         * @brief Non-temporal streamed stores.
         *
         * SIMD values are written to memory using non-temporal (streaming) store instructions,
         * which bypass the cache and minimize pollution for large, linear data writes.
         *
         * Example: _mm256_stream_ps, _mm512_stream_si512, etc.
         */
        Streamed
    };

    /**
     * @enum T_PrefetchHints
     * @brief Specifies locality hints for data prefetching operations.
     *
     * SIMD prefetch hints guide the CPU cache controller on which cache level
     * to populate with the requested data. This optimization can significantly
     * reduce memory latency and prevent cache pollution (evicting useful data)
     * during large streaming operations.
     *
     * These hints map directly to intrinsics like _mm_prefetch.
     */
    enum class T_PrefetchHints {
        /**
         * @brief No prefetching.
         */
        HintNone,

        /**
         * @brief Temporal data: keep in all cache levels (L1, L2, L3).
         *
         * Indicates that the data will be accessed soon and reused. The data
         * is brought into all cache levels effective for the architecture.
         *
         * Maps to: _MM_HINT_T0
         */
        HintT0, 

        /**
         * @brief Temporal data: keep in L2 and higher (L2, L3).
         *
         * Indicates that the data will be accessed, but not immediately, or
         * that L1 space should be preserved for other critical data.
         *
         * Maps to: _MM_HINT_T1
         */
        HintT1,

        /**
         * @brief Temporal data: keep in L3 and higher.
         *
         * Indicates that the data valid but can be served from the Last Level Cache (LLC).
         * Useful to avoid polluting the faster, smaller L1/L2 caches.
         *
         * Maps to: _MM_HINT_T2
         */
        HintT2,

        /**
         * @brief Non-temporal data: minimize cache pollution (NTA).
         *
         * "Non-Temporal Aligned". Fetches data into a non-temporal structure
         * (or L1) with the expectation that it will be used once and then
         * discarded. Prevents evicting useful data from caches.
         *
         * Maps to: _MM_HINT_NTA
         */
        HintNTA
    };

    /**
     * @enum T_ILPPolicy
     * @brief Specifies the Instruction-Level Parallelism (ILP) strategy for SIMD operations.
     *
     * The ILP policy determines how instructions (loads, arithmetic, stores) are scheduled 
     * within unrolled loops. This allows fine-tuning of register pressure and execution port 
     * utilization based on the target microarchitecture and unroll factor.
     *
     * - T_ILPPolicy::Interleaved : Instructions are interleaved (e.g., load-add-store, load-add-store).
     * - T_ILPPolicy::Grouped     : Instructions of the same type are grouped (e.g., all loads, then all adds, then all stores).
     */
    enum class T_ILPPolicy {
        /**
         * @brief Interleaved instruction scheduling.
         *
         * Minimizes register pressure by keeping the live range of SIMD registers short.
         * Relies on the CPU's Out-of-Order (OoO) execution engine to dynamically extract ILP.
         * Recommended for large unroll factors or architectures with limited registers (e.g., AVX2).
         */
        Interleaved,

        /**
         * @brief Grouped instruction scheduling (Explicit ILP).
         *
         * Forces the compiler to issue all instructions of a specific phase (e.g., all loads) 
         * before moving to the next phase (e.g., all arithmetic). Maximizes execution port 
         * saturation early in the pipeline but significantly increases register pressure. 
         * Recommended only for small unroll factors.
         */
        Grouped
    };

    /**
     * @brief Returns true if any SIMD instruction set is enabled at compile time.
     * @return true if SIMD is available; false otherwise.
     */
    inline constexpr bool IsSIMDEnabled() {
        #if STALKER_SIMD_ENABLE != 0 && !defined(STALKER_SIMD_INSTRUCTION_SET_NONE)
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Returns true if SIMD is enabled and at least one AVX capability was detected.
     */
    inline constexpr bool IsSIMDOk() {
        #if defined(STALKER_SIMD_ENABLE) && STALKER_SIMD_ENABLE == 1 && \
            (defined(STALKER_SIMD_AVX2_OK) || defined(STALKER_SIMD_AVX512_OK))
            return true;
        #else
            return false;
        #endif
    }
    
    /**
     * @brief Returns the enumerator for the active SIMD type.
     * @return T_SIMD corresponding to the active SIMD instruction set.
     *
     * @note Returns T_SIMD::None if SIMD is disabled or unsupported.
     * @note Throws a compile-time error if no SIMD instruction set is defined.
     */
    inline constexpr T_SIMD DefaultSIMDType() {
        #if defined(STALKER_SIMD_ENABLE) && STALKER_SIMD_ENABLE != 0 && \
            (defined(STALKER_SIMD_AVX2_OK) || defined(STALKER_SIMD_AVX512_OK))
            #if defined(STALKER_SIMD_INSTRUCTION_SET_AVX512)
                return T_SIMD::AVX512;
            #elif defined(STALKER_SIMD_INSTRUCTION_SET_AVX2)
                return T_SIMD::AVX2;
            #elif defined(STALKER_SIMD_INSTRUCTION_SET_ESP_DSP)
                return T_SIMD::ESP_DSP;
            #elif defined(STALKER_SIMD_INSTRUCTION_SET_NONE)
                return T_SIMD::None;
            #else
                #error "STALKER::Core::Config::DefaultSIMDType: Unknown SIMD instruction set!"
            #endif
        #else
            return T_SIMD::None;
        #endif
    }

    /**
     * @brief Returns the canonical string name for the active SIMD type.
     * @return "avx512", "avx2", "esp-dsp", or "none" (for disabled).
     */
    inline constexpr const char* DefaultSIMDName() {
        switch (DefaultSIMDType()) {
            case T_SIMD::AVX512:  return "avx512";
            case T_SIMD::AVX2:    return "avx2";
            case T_SIMD::ESP_DSP: return "esp-dsp";
            default:                return "none";
        }
    }

    /**
     * @brief Returns the SIMD register width in bits for the active SIMD instruction set.
     * @return Register width (512, 256, 128), or 0 if SIMD is disabled.
     */
    inline constexpr size_t DefaultSIMDRegisterSize() {
        switch (DefaultSIMDType()) {
            case T_SIMD::AVX512:  return 512;
            case T_SIMD::AVX2:    return 256;
            case T_SIMD::ESP_DSP: return 128;
            default:                return 0;
        }
    }

    /**
     * @brief Returns the preferred store type for SIMD memory operations.
     * @return T_SIMDStore::Cached (normal) or T_SIMDStore::Streamed (non-temporal)
     *
     * @details
     * The preferred store policy is selected at compile time using CMake macros:
     *   - @b STALKER_SIMD_STORE_TYPE_CACHE   : Forces Cached stores (default if unset)
     *   - @b STALKER_SIMD_STORE_TYPE_STREAM : Forces Streamed stores
     *
     * Use this function in generic SIMD code to select between normal or streaming stores,
     * for example, to toggle between `_mm256_storeu_ps` and `_mm256_stream_ps` at compile time.
     *
     * @section storetype_examples Example
     * @code{.cpp}
     * if constexpr (DefaultSIMDStore() == T_SIMDStore::Streamed) {
     *     _mm256_stream_ps(dst, values); // Use streaming (non-temporal) store
    * } else {
     *     _mm256_storeu_ps(dst, values); // Use normal cacheable store
     * }
     * @endcode
     */
    inline constexpr T_SIMDStore DefaultSIMDStore() {
        #if defined(STALKER_SIMD_STORE_TYPE_CACHE)
            return T_SIMDStore::Cached;
        #elif defined(STALKER_SIMD_STORE_TYPE_STREAM)
            return T_SIMDStore::Streamed;
        #else
            return T_SIMDStore::Cached;
        #endif
    }

    /**
     * @brief Returns the default prefetch locality hint.
     * @return T_PrefetchHints enum value.
     */
    inline constexpr T_PrefetchHints DefaultPrefetchHint() {
        #if defined(STALKER_SIMD_PREFETCH_HINT)
            return T_PrefetchHints::STALKER_SIMD_PREFETCH_HINT;
        #else
            return T_PrefetchHints::HintNone;
        #endif
    }

    /**
     * @brief Returns the default ILP policy.
     * @return T_ILPPolicy::Interleaved or T_ILPPolicy::Grouped
     *
     * @details
     * The preferred ILP policy is selected at compile time using CMake macros:
     *   - @b STALKER_SIMD_ILP_POLICY_INTERLEAVED : Forces Interleaved loads/stores (default)
     *   - @b STALKER_SIMD_ILP_POLICY_GROUPED     : Forces Grouped loads/stores
     */
    inline constexpr T_ILPPolicy DefaultILPPolicy() {
        #if defined(STALKER_SIMD_ILP_POLICY_GROUPED)
            return T_ILPPolicy::Grouped;
        #elif defined(STALKER_SIMD_ILP_POLICY_INTERLEAVED)
            return T_ILPPolicy::Interleaved;
        #else
            return T_ILPPolicy::Interleaved;
        #endif
    }
} // namespace Stalker::Core::Config

