//
// Created by hal9000 on 4/22/24.
//

#ifndef STALKER_STLKR_SIMD_DATAOPERATIONS_H
#define STALKER_STLKR_SIMD_DATAOPERATIONS_H

#include "STLKR_SIMD.h"

namespace STLKR_LinearAlgebra{
    
    template <typename T, size_t size>
    class STLKR_SIMD_DataOperations {
        
    public:
        
        constexpr static void deepcopy(T *destination, T *source, const STLKR_SIMD &simdPolicy) {
            const auto &avxVersion = simdPolicy.getAVXType();
            if (avxVersion == SIMD_AVXType::AVX || avxVersion == SIMD_AVXType::AVX_2) {
                auto registerSize = STLKR_SIMD_DataTypeSize.at(type_index(typeid(T)));
                for (unsigned i = 0; i < size - (size % registerSize); i += registerSize) {
                    if constexpr (is_same_v<T, float>) {
                        __m256 va = _mm256_loadu_ps(source + i);
                        _mm256_storeu_ps(destination + i, va);
                    }
                    else if constexpr (is_same_v<T, double>) {
                        __m256d va = _mm256_loadu_pd(source + i);
                        _mm256_storeu_pd(destination + i, va);
                    }
                    else if constexpr (is_same_v<T, int> || is_same_v<T, unsigned> || is_same_v<T, short>) {
                        __m256i va = _mm256_loadu_si256((__m256i *)(source + i));
                        _mm256_storeu_si256((__m256i *)(destination + i), va);
                    }
                    else throw runtime_error("SIMD version not supported");
                }
            } else throw runtime_error("SIMD version not supported");
        }
        
    private:
    };
    
    
}
#endif //STALKER_STLKR_SIMD_DATAOPERATIONS_H
