//
// Created by hal9000 on 7/9/24.
//

#ifndef STALKER_SIMD_MATHTRAITS_H
#define STALKER_SIMD_MATHTRAITS_H
#include "MemoryTraits.h"
#include <immintrin.h>
#include <algorithm>
#include <numeric>

template<typename T, unsigned int unrollFactor>
struct MathTraits;

// Float Specialization
template<unsigned int unrollFactor>
struct MathTraits<float, unrollFactor> {
    using T_avx2 = __m256;
    using T_data = float;
    static constexpr unsigned registerSize = FLOAT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(float);

    static constexpr inline void add(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollAdd<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void subtract(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollSubtract<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void multiply(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollMultiply<unrollFactor>(a, b, result, scale1, scale2);
    }
    
    static constexpr inline void scale(T_avx2* data, const T_avx2* scale){
        _scale<unrollFactor>(data, scale);
    }

    static constexpr inline void sum(const T_avx2 *data, T_data *result){
        _sum<unrollFactor>(data, result);
    }

    static constexpr inline void dotProduct(const T_avx2* a, const T_avx2* b, T_data * result){;
        _dotProduct<unrollFactor>(a, b, result);
    }

private:
    template<unsigned iUnroll>
    static constexpr inline void _unrollAdd(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_fmadd_ps(*(a + iUnroll - 1), *scale1, _mm256_mul_ps(*(b + iUnroll - 1), *scale2));
            _unrollAdd<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollSubtract(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_fmsub_ps(*(a + iUnroll - 1), *scale1, _mm256_mul_ps(*(b + iUnroll - 1), *scale2));
            _unrollSubtract<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollMultiply(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_mul_ps(_mm256_mul_ps(*(a + iUnroll - 1), *scale1), _mm256_mul_ps(*(b + iUnroll - 1), *scale2));
            _unrollMultiply<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }
    
    template<unsigned iUnroll>
    static constexpr inline void _scale(T_avx2* data, const T_avx2* scale){
        if constexpr (iUnroll > 0) {
            *(data + iUnroll - 1) = _mm256_mul_ps(*(data + iUnroll - 1), *scale);
            _scale<iUnroll - 1>(data, scale);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _sum(const T_avx2* data, T_data* result) {
        if constexpr (iUnroll > 0) {
            __m128 hi = _mm256_extractf128_ps(*(data + iUnroll - 1), 1);
            __m128 lo = _mm256_castps256_ps128(*(data + iUnroll - 1));
            __m128 sum128 = _mm_add_ps(lo, hi);

            // Horizontal add to get the sum of all elements in the 128-bit lanes
            sum128 = _mm_hadd_ps(sum128, sum128);
            sum128 = _mm_hadd_ps(sum128, sum128);

            *result += _mm_cvtss_f32(sum128);
            _sum<iUnroll - 1>(data, result);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _dotProduct(const T_avx2* a, const T_avx2* b, T_data* result) {
        if constexpr (iUnroll > 0) {
            T_avx2 temp = _mm256_mul_ps(*(a + iUnroll - 1), *(b + iUnroll - 1));

            __m128 hi = _mm256_extractf128_ps(temp, 1);
            __m128 lo = _mm256_castps256_ps128(temp);
            __m128 sum128 = _mm_add_ps(lo, hi);

            // Horizontal add to get the sum of all elements in the 128-bit lanes
            sum128 = _mm_hadd_ps(sum128, sum128);
            sum128 = _mm_hadd_ps(sum128, sum128);

            *result += _mm_cvtss_f32(sum128);
            _dotProduct<iUnroll - 1>(a, b, result);
        }
    }
    
};

// Double Specialization
template<unsigned int unrollFactor>
struct MathTraits<double, unrollFactor> {
    using T_avx2 = __m256d;
    using T_data = double;
    static constexpr unsigned registerSize = DOUBLE_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(double);

    static constexpr inline void add(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollAdd<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void subtract(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollSubtract<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void multiply(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollMultiply<unrollFactor>(a, b, result, scale1, scale2);
    }
    
    static constexpr inline void scale(T_avx2* data, const T_avx2* scale){
        _scale<unrollFactor>(data, scale);
    }
    
    static constexpr inline void sum(const T_avx2 *data, T_data *result){
        _sum<unrollFactor>(data, result);
    }
    
    static constexpr inline void dotProduct(const T_avx2* a, const T_avx2* b, T_data * result){;
        _dotProduct<unrollFactor>(a, b, result);
    }

private:
    template<unsigned iUnroll>
    static constexpr inline void _unrollAdd(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_fmadd_pd(*(a + iUnroll - 1), *scale1, _mm256_mul_pd(*(b + iUnroll - 1), *scale2));
            _unrollAdd<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollSubtract(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_fmsub_pd(*(a + iUnroll - 1), *scale1, _mm256_mul_pd(*(b + iUnroll - 1), *scale2));
            _unrollSubtract<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollMultiply(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_mul_pd(_mm256_mul_pd(*(a + iUnroll - 1), *scale1), _mm256_mul_pd(*(b + iUnroll - 1), *scale2));
            _unrollMultiply<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }
    
    template<unsigned iUnroll>
    static constexpr inline void _scale(T_avx2* data, const T_avx2* scale){
        if constexpr (iUnroll > 0) {
            *(data + iUnroll - 1) = _mm256_mul_pd(*(data + iUnroll - 1), *scale);
            _scale<iUnroll - 1>(data, scale);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _sum(const T_avx2* data, T_data* result) {
        if constexpr (iUnroll > 0) {
            __m128d hi = _mm256_extractf128_pd(*(data + iUnroll - 1), 1);
            __m128d lo = _mm256_castpd256_pd128(*(data + iUnroll - 1));
            __m128d sum128 = _mm_add_pd(lo, hi);

            // Horizontal add to get the sum of all elements in the 128-bit lanes
            sum128 = _mm_hadd_pd(sum128, sum128);

            *result += _mm_cvtsd_f64(sum128);
            _sum<iUnroll - 1>(data, result);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _dotProduct(const T_avx2* a, const T_avx2* b, T_data* result) {
        if constexpr (iUnroll > 0) {
            T_avx2 temp = _mm256_mul_pd(*(a + iUnroll - 1), *(b + iUnroll - 1));

            __m128d hi = _mm256_extractf128_pd(temp, 1);
            __m128d lo = _mm256_castpd256_pd128(temp);
            __m128d sum128 = _mm_add_pd(lo, hi);

            // Horizontal add to get the sum of all elements in the 128-bit lanes
            sum128 = _mm_hadd_pd(sum128, sum128);

            *result += _mm_cvtsd_f64(sum128);
            _dotProduct<iUnroll - 1>(a, b, result);
        }
    }
    
};

// Int Specialization
template<unsigned int unrollFactor>
struct MathTraits<int, unrollFactor> {
    using T_avx2 = __m256i;
    using T_data = int;
    static constexpr unsigned registerSize = INT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(int);

    static constexpr inline void add(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollAdd<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void subtract(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollSubtract<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void multiply(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollMultiply<unrollFactor>(a, b, result, scale1, scale2);
    }
    
    static constexpr inline void scale(T_avx2* data, const T_avx2* scale){
        _scale<unrollFactor>(data, scale);
    }
    
    static constexpr inline void sum(const T_avx2 *data, T_data *result){
        _sum<unrollFactor>(data, result);
    }

    static constexpr inline void dotProduct(const T_avx2* a, const T_avx2* b, T_data * result){;
        _dotProduct<unrollFactor>(a, b, result);
    }
    
private:
    template<unsigned iUnroll>
    static constexpr inline void _unrollAdd(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_add_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollAdd<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollSubtract(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_sub_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollSubtract<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollMultiply(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_mullo_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollMultiply<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }
    
    template<unsigned iUnroll>
    static constexpr inline void _scale(T_avx2* data, const T_avx2* scale){
        if constexpr (iUnroll > 0) {
            *(data + iUnroll - 1) = _mm256_mullo_epi32(*(data + iUnroll - 1), *scale);
            _scale<iUnroll - 1>(data, scale);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _sum(const T_avx2* data, T_data* result) {
        if constexpr (iUnroll > 0) {
            // Extract the high and low 128-bit lanes
            __m128i hi = _mm256_extracti128_si256(*(data + iUnroll - 1), 1);
            __m128i lo = _mm256_castsi256_si128(*(data + iUnroll - 1));
            __m128i sum128 = _mm_add_epi32(lo, hi);

            // Horizontal add to get the sum of all elements in the 128-bit lanes
            sum128 = _mm_hadd_epi32(sum128, sum128);
            sum128 = _mm_hadd_epi32(sum128, sum128);

            *result += _mm_cvtsi128_si32(sum128);
            _sum<iUnroll - 1>(data, result);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _dotProduct(const T_avx2* a, const T_avx2* b, T_data* result) {
        if constexpr (iUnroll > 0) {
            T_avx2 temp = _mm256_mullo_epi32(*(a + iUnroll - 1), *(b + iUnroll - 1));

            __m128i hi = _mm256_extracti128_si256(temp, 1);
            __m128i lo = _mm256_castsi256_si128(temp);
            __m128i sum128 = _mm_add_epi32(lo, hi);

            // Horizontal add to get the sum of all elements in the 128-bit lanes
            sum128 = _mm_hadd_epi32(sum128, sum128);
            sum128 = _mm_hadd_epi32(sum128, sum128);

            *result += _mm_cvtsi128_si32(sum128);
            _dotProduct<iUnroll - 1>(a, b, result);
        }
    }
    
};

// Unsigned Specialization
template<unsigned int unrollFactor>
struct MathTraits<unsigned, unrollFactor> {
    using T_avx2 = __m256i;
    using T_data = unsigned;
    static constexpr unsigned registerSize = UNSIGNED_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(unsigned);

    static constexpr inline void add(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollAdd<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void subtract(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollSubtract<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void multiply(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollMultiply<unrollFactor>(a, b, result, scale1, scale2);
    }
    
    static constexpr inline void scale(T_avx2* data, const T_avx2* scale){
        _scale<unrollFactor>(data, scale);
    }
    
    static constexpr inline void sum(const T_avx2 *data, T_data *result){
        _sum<unrollFactor>(data, result);
    }
    
    static constexpr inline void dotProduct(const T_avx2* a, const T_avx2* b, T_data * result){;
        _dotProduct<unrollFactor>(a, b, result);
    }

private:
    template<unsigned iUnroll>
    static constexpr inline void _unrollAdd(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_add_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollAdd<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollSubtract(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_sub_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollSubtract<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollMultiply(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_mullo_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollMultiply<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }
    
    template<unsigned iUnroll>
    static constexpr inline void _scale(T_avx2* data, const T_avx2* scale){
        if constexpr (iUnroll > 0) {
            *(data + iUnroll - 1) = _mm256_mullo_epi32(*(data + iUnroll - 1), *scale);
            _scale<iUnroll - 1>(data, scale);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _sum(const T_avx2* data, T_data* result) {
        if constexpr (iUnroll > 0) {
            // Extract the high and low 128-bit lanes
            __m128i hi = _mm256_extracti128_si256(*(data + iUnroll - 1), 1);
            __m128i lo = _mm256_castsi256_si128(*(data + iUnroll - 1));
            __m128i sum128 = _mm_add_epi32(lo, hi);

            // Horizontal add to get the sum of all elements in the 128-bit lanes
            sum128 = _mm_hadd_epi32(sum128, sum128);
            sum128 = _mm_hadd_epi32(sum128, sum128);

            *result += _mm_cvtsi128_si32(sum128);
            _sum<iUnroll - 1>(data, result);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _dotProduct(const T_avx2* a, const T_avx2* b, T_data* result) {
        if constexpr (iUnroll > 0) {
            T_avx2 temp = _mm256_mullo_epi32(*(a + iUnroll - 1), *(b + iUnroll - 1));

            __m128i hi = _mm256_extracti128_si256(temp, 1);
            __m128i lo = _mm256_castsi256_si128(temp);
            __m128i sum128 = _mm_add_epi32(lo, hi);

            // Horizontal add to get the sum of all elements in the 128-bit lanes
            sum128 = _mm_hadd_epi32(sum128, sum128);
            sum128 = _mm_hadd_epi32(sum128, sum128);

            *result += static_cast<T_data>(_mm_cvtsi128_si32(sum128));
            _dotProduct<iUnroll - 1>(a, b, result);
        }
    }

};

// Short Specialization
template<unsigned int unrollFactor>
struct MathTraits<short, unrollFactor> {
    using T_avx2 = __m256i;
    using T_data = short;
    static constexpr unsigned registerSize = SHORT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(short);

    static constexpr inline void add(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollAdd<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void subtract(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollSubtract<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void multiply(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        _unrollMultiply<unrollFactor>(a, b, result, scale1, scale2);
    }
    
    static constexpr inline void scale(T_avx2* data, const T_avx2* scale){
        _scale<unrollFactor>(data, scale);
    }
    
    static constexpr inline void sum(const T_avx2* data, T_data* result){
        _sum<unrollFactor>(data, result);
    }

    static constexpr inline void dotProduct(const T_avx2* a, const T_avx2* b, T_data * result){;
        _dotProduct<unrollFactor>(a, b, result);
    }

private:
    template<unsigned iUnroll>
    static constexpr inline void _unrollAdd(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_add_epi16(_mm256_mullo_epi16(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi16(*(b + iUnroll - 1), *scale2));
            _unrollAdd<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollSubtract(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_sub_epi16(_mm256_mullo_epi16(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi16(*(b + iUnroll - 1), *scale2));
            _unrollSubtract<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollMultiply(const T_avx2* a, const T_avx2* b, T_avx2* result, const T_avx2* scale1, const T_avx2* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_mullo_epi16(_mm256_mullo_epi16(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi16(*(b + iUnroll - 1), *scale2));
            _unrollMultiply<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }
    
    template<unsigned iUnroll>
    static constexpr inline void _scale(T_avx2* data, const T_avx2* scale){
        if constexpr (iUnroll > 0) {
            *(data + iUnroll - 1) = _mm256_mullo_epi16(*(data + iUnroll - 1), *scale);
            _scale<iUnroll - 1>(data, scale);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _sum(const T_avx2* data, T_data* result) {
        if constexpr (iUnroll > 0) {
            // Extract the high and low 128-bit lanes
            __m128i hi = _mm256_extracti128_si256(*(data + iUnroll - 1), 1);
            __m128i lo = _mm256_castsi256_si128(*(data + iUnroll - 1));

            // Horizontal add within each 128-bit lane
            __m128i sum128 = _mm_add_epi16(lo, hi);
            sum128 = _mm_hadd_epi16(sum128, sum128);
            sum128 = _mm_hadd_epi16(sum128, sum128);
            sum128 = _mm_hadd_epi16(sum128, sum128);

            // Extract the lower 16 bits of sum128 and add to the result
            *result += _mm_extract_epi16(sum128, 0);

            _sum<iUnroll - 1>(data, result);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _dotProduct(const T_avx2* a, const T_avx2* b, T_data* result) {
        if constexpr (iUnroll > 0) {
            T_avx2 temp = _mm256_mullo_epi16(*(a + iUnroll - 1), *(b + iUnroll - 1));

            __m128i hi = _mm256_extracti128_si256(temp, 1);
            __m128i lo = _mm256_castsi256_si128(temp);
            __m128i sum128 = _mm_add_epi16(lo, hi);

            // Horizontal add to get the sum of all elements in the 128-bit lanes
            sum128 = _mm_hadd_epi16(sum128, sum128);
            sum128 = _mm_hadd_epi16(sum128, sum128);
            sum128 = _mm_hadd_epi16(sum128, sum128);

            *result += _mm_extract_epi16(sum128, 0);
            _dotProduct<iUnroll - 1>(a, b, result);
        }
    }

};
#endif //STALKER_SIMD_MATHTRAITS_H
