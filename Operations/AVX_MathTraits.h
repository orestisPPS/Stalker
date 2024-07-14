//
// Created by hal9000 on 7/9/24.
//

#ifndef STALKER_AVX_MATHTRAITS_H
#define STALKER_AVX_MATHTRAITS_H
#include "AVX_MemoryTraits.h"
#include <immintrin.h>

template<typename T, unsigned int unrollFactor>
struct AVX_MathTraits;

// Float Specialization
template<unsigned int unrollFactor>
struct AVX_MathTraits<float, unrollFactor> {
    using AVXRegisterType = __m256;
    using DataType = float;
    static constexpr unsigned AVXRegisterSize = FLOAT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(float);
    
    static constexpr inline DataType dotProduct(const AVXRegisterType* data1, const AVXRegisterType* data2) {
        DataType result = 0;
        _dotProduct<unrollFactor>(data1, data2, result);
        return result;
    }
    
    
//    template<unsigned powerValue>
//    static constexpr inline void power(const AVXRegisterType* data) {
//        _unrollPower<unrollFactor, powerValue>(data);
//    }
   
    
private:

    template<unsigned iUnroll>
    static constexpr inline void _dotProduct(const AVXRegisterType* data1, const AVXRegisterType* data2, DataType & result) {
        if constexpr (iUnroll > 0) {
            // Load the AVX registers with data from the input arrays
            AVXRegisterType vec1 = *(data1 + iUnroll - 1);
            AVXRegisterType vec2 = *(data2 + iUnroll - 1);

            // Multiply corresponding elements of the two vectors
            AVXRegisterType mulRes = _mm256_mul_ps(vec1, vec2);

            // Sum the results of the multiplications within the register
            __m128 hi = _mm256_extractf128_ps(mulRes, 1);
            __m128 lo = _mm256_castps256_ps128(mulRes);
            __m128 sum128 = _mm_add_ps(lo, hi);
            sum128 = _mm_hadd_ps(sum128, sum128);
            sum128 = _mm_hadd_ps(sum128, sum128);

            // Accumulate the sum from this iteration with the rest
            result = _mm_cvtss_f32(sum128) + _dotProduct<iUnroll - 1>(data1, data2, result);
        }
        else return;
    }
    
    template<unsigned iUnroll, unsigned iPower>
    static constexpr inline void _unrollPower(const AVXRegisterType* data) {
        if constexpr (iUnroll > 0) {
            _raisePower<iPower>(data);
            _unrollPower<iUnroll - 1, iPower>(data);
        }
        else return;
    }
    
    template<unsigned iPower>
    static constexpr inline void _raisePower(AVXRegisterType* data) {
        if constexpr (iPower > 0) {
            *data = _mm256_mul_ps(*data, *data);
            _raisePower<iPower - 1>(data);
        }
        else return;
    }
};
#endif //STALKER_AVX_MATHTRAITS_H
