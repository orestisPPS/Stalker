//
// Created by hal9000 on 8/11/24.
//

#ifndef STALKER_SIMD_TRAITS_H
#define STALKER_SIMD_TRAITS_H
#include <immintrin.h>
#include <algorithm>
#include <numeric>
#include "AVX_MemoryTraits.h"
#include "SIMD_MathTraits.h"
#endif //STALKER_SIMD_TRAITS_H

enum class SIMD_StoreType {
    Temporal,        // For _mm256_store_* (cached store)
    NonTemporal      // For _mm256_stream_* (non-cached streaming store)
};
template<typename T, unsigned unrollFactor, SIMD_StoreType storeType>
struct SIMD_Configuration {
    static constexpr unsigned UnrollFactor = unrollFactor;
    static constexpr SIMD_StoreType StoreType = storeType;
    static constexpr AVX_MemoryTraits Memory = AVX_MemoryTraits<T, unrollFactor>();
    static constexpr SIMD_MathTraits Math = SIMD_MathTraits<T, unrollFactor>();
};