//
// Created by hal9000 on 7/9/24.
//

#ifndef STALKER_SIMD_MATHOPERATIONS_H
#define STALKER_SIMD_MATHOPERATIONS_H

#include <utility>
#include "SIMDTypeTraits.h"
#include "SIMDMathOperations.h"

template<typename T, SIMDType Type> struct SIMDMathOperations; 
template<> struct SIMDMathOperations<float, SIMDType::AVX2>;
template<> struct SIMDMathOperations<int, SIMDType::AVX2>;
template<> struct SIMDMathOperations<short, SIMDType::AVX2>;
template<> struct SIMDMathOperations<unsigned, SIMDType::AVX2>;
template<> struct SIMDMathOperations<double, SIMDType::AVX2>;

// CRTP Base
template<typename T, SIMDType Type, SIMDStoreType Policy, typename Child>
struct SIMDMathOperationsBase {
    
    using T_simd = typename SIMDTypeTraits<T, Type>::typeSIMD;
    using T_data = typename SIMDTypeTraits<T, Type>::typeData;

public:
    static constexpr unsigned blockSize = SIMDTypeTraits<T, Type>::BlockSize;
    static constexpr unsigned registerSize = SIMDTypeTraits<T, Type>::RegisterSize;
    
    constexpr inline static void add(T_data *data1, T_data  *data2, T_data *result, T_data scale1, T_data scale2, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result[i] = data1[i] * scale1 + data2[i] * scale2;
    }

    constexpr inline static void add(T_data *data1, T_data *data2, T_data *result, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result[i] = data1[i] + data2[i];
    }

    constexpr inline static void subtract(T_data *data1, T_data  *data2, T_data *result, T_data scale1, T_data scale2, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result[i] = data1[i] * scale1 - data2[i] * scale2;
    }

    constexpr inline static void subtract(T_data *data1, T_data *data2, T_data *result, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result[i] = data1[i] - data2[i];
    }

    constexpr inline static void multiply(T_data *data1, T_data  *data2, T_data *result, T_data scale1, T_data scale2, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result[i] = data1[i] * scale1 * data2[i] * scale2;
    }

    constexpr inline static void multiply(T_data *data1, T_data *data2, T_data *result, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result[i] = data1[i] * data2[i];
    }

    constexpr inline static void divide(T_data *data1, T_data  *data2, T_data *result, T_data scale1, T_data scale2, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result[i] = data1[i] * scale1 / data2[i] * scale2;
    }

    constexpr inline static void divide(T_data *data1, T_data *data2, T_data *result, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result[i] = data1[i] / data2[i];
    }

    
    static constexpr inline void scale(T_data* __restrict data, const T_data scale, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            data[i] *= scale;
    }

    static constexpr inline void scale(T_data* __restrict data, T_data* __restrict result, const T_data scale, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result[i] = data[i] * scale;
    }

    static constexpr inline T_data sum(const T_data* __restrict data, unsigned size){
        auto limit = size - (size % blockSize);
        T_data result{};
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result += data[i];
        return result;
    }

    static constexpr inline T_data dot(const T_data* __restrict a, const T_data* __restrict b, unsigned size){
        auto limit = size - (size % blockSize);
        T_data result{};
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result += a[i] * b[i];
        return result;
    }

    static constexpr inline T_data dot(const T_data* __restrict a, const T_data* __restrict b, T_data scaleA, T_data scaleB, unsigned size){
        auto limit = size - (size % blockSize);
        T_data result{};
        for (size_t i = 0; i < limit; i += blockSize) {

        }
        for (size_t i = limit; i < size; i++)
            result += a[i] * scaleA * b[i] * scaleB;
        return result;
    }


};

template<SIMDStoreType Policy>
struct SIMDMathOperations<double, SIMDType::AVX2>
    : public SIMDMathOperationsBase<double, SIMDType::AVX2, SIMDMathOperations<double, SIMDType::AVX2>> {

    using Base = SIMDMathOperationsBase<double, SIMDType::AVX2, SIMDMathOperations<double, SIMDType::AVX2>>;

private:

    template<size_t... Is>
    static constexpr inline void _add(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result,
                                            const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
        ((result[Is] = _mm256_fmadd_pd(a[Is], *scale1, _mm256_mul_pd(b[Is], *scale2))), ...);
    }

    template<size_t... Is>
    static constexpr inline void _subtract(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
        ((result[Is] = _mm256_fmsub_pd(a[Is], *scale1, _mm256_mul_pd(b[Is], *scale2))), ...);
    }

    template<size_t... Is>
    static constexpr inline void _multiply(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
        ((result[Is] = _mm256_mul_pd(_mm256_mul_pd(a[Is], *scale1), _mm256_mul_pd(b[Is], *scale2))), ...);
    }
    
    template<size_t... Is>
    static constexpr inline void _scale(Base::T_simd* data, const Base::T_simd* scale, std::index_sequence<Is...>){
        ((data[Is] = _mm256_mul_pd(data[Is], *scale)), ...);
    }

    template<size_t... Is>
    static constexpr inline void _sum(const Base::T_simd* data, Base::T_data* result, std::index_sequence<Is...>) {
        (([&]{
            __m128d hi = _mm256_extractf128_pd(data[Is], 1);
            __m128d lo = _mm256_castpd256_pd128(data[Is]);
            __m128d sum128 = _mm_add_pd(lo, hi);
            sum128 = _mm_hadd_pd(sum128, sum128);
            *result += _mm_cvtsd_f64(sum128);
        }()), ...);
    }

    template<size_t... Is>
    static constexpr inline void _dotProduct(const Base::T_simd* a, const Base::T_simd* b, Base::T_data* result, std::index_sequence<Is...>) {
        (([&]{
            Base::T_simd temp = _mm256_mul_pd(a[Is], b[Is]);
            __m128d hi = _mm256_extractf128_pd(temp, 1);
            __m128d lo = _mm256_castpd256_pd128(temp);
            __m128d sum128 = _mm_add_pd(lo, hi);
            sum128 = _mm_hadd_pd(sum128, sum128);
            *result += _mm_cvtsd_f64(sum128);
        }()), ...);
    }
    
};

// // Double Specialization
// template<>
// struct SIMDMathOperations<float, SIMDType::AVX2>
//     : public SIMDMathOperationsBase<float, SIMDType::AVX2, SIMDMathOperations<float, SIMDType::AVX2>> {
    
//     using Base = SIMDMathOperationsBase<float, SIMDType::AVX2, SIMDMathOperations<float, SIMDType::AVX2>>;

// private:

//     template<size_t... Is>
//     static constexpr inline void _add(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_fmadd_ps(a[Is], *scale1, _mm256_mul_ps(b[Is], *scale2))), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _subtract(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_fmsub_ps(a[Is], *scale1, _mm256_mul_ps(b[Is], *scale2))), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _multiply(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_mul_ps(_mm256_mul_ps(a[Is], *scale1), _mm256_mul_ps(b[Is], *scale2))), ...);
//     }
    
//     template<size_t... Is>
//     static constexpr inline void _scale(Base::T_simd* data, const Base::T_simd* scale, std::index_sequence<Is...>){
//         ((data[Is] = _mm256_mul_ps(data[Is], *scale)), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _sum(const Base::T_simd* data, Base::T_data* result, std::index_sequence<Is...>) {
//         (([&]{
//             __m128 hi = _mm256_extractf128_ps(data[Is], 1);
//             __m128 lo = _mm256_castps256_ps128(data[Is]);
//             __m128 sum128 = _mm_add_ps(lo, hi);

//             // Horizontal add to get the sum of all elements in the 128-bit lanes
//             sum128 = _mm_hadd_ps(sum128, sum128);
//             sum128 = _mm_hadd_ps(sum128, sum128);

//             *result += _mm_cvtss_f32(sum128);
//         }()), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _dotProduct(const Base::T_simd* a, const Base::T_simd* b, Base::T_data* result, std::index_sequence<Is...>) {
//         (([&]{
//             Base::T_simd temp = _mm256_mul_ps(a[Is], b[Is]);

//             __m128 hi = _mm256_extractf128_ps(temp, 1);
//             __m128 lo = _mm256_castps256_ps128(temp);
//             __m128 sum128 = _mm_add_ps(lo, hi);

//             // Horizontal add to get the sum of all elements in the 128-bit lanes
//             sum128 = _mm_hadd_ps(sum128, sum128);
//             sum128 = _mm_hadd_ps(sum128, sum128);

//             *result += _mm_cvtss_f32(sum128);
//         }()), ...);
//     }
    
// };

// // Int Specialization
// template<>
// struct SIMDMathOperations<int, SIMDType::AVX2>
//     : public SIMDMathOperationsBase<int, SIMDType::AVX2, SIMDMathOperations<int, SIMDType::AVX2>> {
//     using Base = SIMDMathOperationsBase<int, SIMDType::AVX2, SIMDMathOperations<int, SIMDType::AVX2>>;

// private:
//     template<size_t... Is>
//     static constexpr inline void _add(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_add_epi32(_mm256_mullo_epi32(a[Is], *scale1), _mm256_mullo_epi32(b[Is], *scale2))), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _subtract(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_sub_epi32(_mm256_mullo_epi32(a[Is], *scale1), _mm256_mullo_epi32(b[Is], *scale2))), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _multiply(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_mullo_epi32(_mm256_mullo_epi32(a[Is], *scale1), _mm256_mullo_epi32(b[Is], *scale2))), ...);
//     }
    
//     template<size_t... Is>
//     static constexpr inline void _scale(Base::T_simd* data, const Base::T_simd* scale, std::index_sequence<Is...>){
//         ((data[Is] = _mm256_mullo_epi32(data[Is], *scale)), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _sum(const Base::T_simd* data, Base::T_data* result, std::index_sequence<Is...>) {
//         (([&]{
//             // Extract the high and low 128-bit lanes
//             __m128i hi = _mm256_extracti128_si256(data[Is], 1);
//             __m128i lo = _mm256_castsi256_si128(data[Is]);
//             __m128i sum128 = _mm_add_epi32(lo, hi);

//             // Horizontal add to get the sum of all elements in the 128-bit lanes
//             sum128 = _mm_hadd_epi32(sum128, sum128);
//             sum128 = _mm_hadd_epi32(sum128, sum128);

//             *result += _mm_cvtsi128_si32(sum128);
//         }()), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _dotProduct(const Base::T_simd* a, const Base::T_simd* b, Base::T_data* result, std::index_sequence<Is...>) {
//         (([&]{
//             Base::T_simd temp = _mm256_mullo_epi32(a[Is], b[Is]);

//             __m128i hi = _mm256_extracti128_si256(temp, 1);
//             __m128i lo = _mm256_castsi256_si128(temp);
//             __m128i sum128 = _mm_add_epi32(lo, hi);

//             // Horizontal add to get the sum of all elements in the 128-bit lanes
//             sum128 = _mm_hadd_epi32(sum128, sum128);
//             sum128 = _mm_hadd_epi32(sum128, sum128);

//             *result += _mm_cvtsi128_si32(sum128);
//         }()), ...);
//     }
    
// };

// // Unsigned Specialization
// template<>
// struct SIMDMathOperations<short, SIMDType::AVX2>
//     : public SIMDMathOperationsBase<short, SIMDType::AVX2, SIMDMathOperations<short, SIMDType::AVX2>> {
//     using Base = SIMDMathOperationsBase<short, SIMDType::AVX2, SIMDMathOperations<short, SIMDType::AVX2>>;

// private:
//     template<size_t... Is>
//     static constexpr inline void _add(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_add_epi32(_mm256_mullo_epi32(a[Is], *scale1), _mm256_mullo_epi32(b[Is], *scale2))), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _subtract(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_sub_epi32(_mm256_mullo_epi32(a[Is], *scale1), _mm256_mullo_epi32(b[Is], *scale2))), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _multiply(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_mullo_epi32(_mm256_mullo_epi32(a[Is], *scale1), _mm256_mullo_epi32(b[Is], *scale2))), ...);
//     }
    
//     template<size_t... Is>
//     static constexpr inline void _scale(Base::T_simd* data, const Base::T_simd* scale, std::index_sequence<Is...>){
//         ((data[Is] = _mm256_mullo_epi32(data[Is], *scale)), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _sum(const Base::T_simd* data, Base::T_data* result, std::index_sequence<Is...>) {
//         (([&]{
//             // Extract the high and low 128-bit lanes
//             __m128i hi = _mm256_extracti128_si256(data[Is], 1);
//             __m128i lo = _mm256_castsi256_si128(data[Is]);
//             __m128i sum128 = _mm_add_epi32(lo, hi);

//             // Horizontal add to get the sum of all elements in the 128-bit lanes
//             sum128 = _mm_hadd_epi32(sum128, sum128);
//             sum128 = _mm_hadd_epi32(sum128, sum128);

//             *result += _mm_cvtsi128_si32(sum128);
//         }()), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _dotProduct(const Base::T_simd* a, const Base::T_simd* b, Base::T_data* result, std::index_sequence<Is...>) {
//         (([&]{
//             Base::T_simd temp = _mm256_mullo_epi32(a[Is], b[Is]);

//             __m128i hi = _mm256_extracti128_si256(temp, 1);
//             __m128i lo = _mm256_castsi256_si128(temp);
//             __m128i sum128 = _mm_add_epi32(lo, hi);

//             // Horizontal add to get the sum of all elements in the 128-bit lanes
//             sum128 = _mm_hadd_epi32(sum128, sum128);
//             sum128 = _mm_hadd_epi32(sum128, sum128);

//             *result += static_cast<Base::T_data>(_mm_cvtsi128_si32(sum128));
//         }()), ...);
//     }

// };

// // Short Specialization
// template<>
// struct SIMDMathOperations<unsigned, SIMDType::AVX2>
//     : public SIMDMathOperationsBase<unsigned, SIMDType::AVX2, SIMDMathOperations<unsigned, SIMDType::AVX2>> {
//     using Base = SIMDMathOperationsBase<unsigned, SIMDType::AVX2, SIMDMathOperations<unsigned, SIMDType::AVX2>>;

// private:
//     template<size_t... Is>
//     static constexpr inline void _add(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_add_epi16(_mm256_mullo_epi16(a[Is], *scale1), _mm256_mullo_epi16(b[Is], *scale2))), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _subtract(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_sub_epi16(_mm256_mullo_epi16(a[Is], *scale1), _mm256_mullo_epi16(b[Is], *scale2))), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _multiply(const Base::T_simd* a, const Base::T_simd* b,Base::T_simd* result, const Base::T_simd* scale1, const Base::T_simd* scale2, std::index_sequence<Is...>) {
//         ((result[Is] = _mm256_mullo_epi16(_mm256_mullo_epi16(a[Is], *scale1), _mm256_mullo_epi16(b[Is], *scale2))), ...);
//     }
    
//     template<size_t... Is>
//     static constexpr inline void _scale(Base::T_simd* data, const Base::T_simd* scale, std::index_sequence<Is...>){
//         ((data[Is] = _mm256_mullo_epi16(data[Is], *scale)), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _sum(const Base::T_simd* data, Base::T_data* result, std::index_sequence<Is...>) {
//         (([&]{
//             // Extract the high and low 128-bit lanes
//             __m128i hi = _mm256_extracti128_si256(data[Is], 1);
//             __m128i lo = _mm256_castsi256_si128(data[Is]);

//             // Horizontal add within each 128-bit lane
//             __m128i sum128 = _mm_add_epi16(lo, hi);
//             sum128 = _mm_hadd_epi16(sum128, sum128);
//             sum128 = _mm_hadd_epi16(sum128, sum128);
//             sum128 = _mm_hadd_epi16(sum128, sum128);

//             // Extract the lower 16 bits of sum128 and add to the result
//             *result += _mm_extract_epi16(sum128, 0);
//         }()), ...);
//     }

//     template<size_t... Is>
//     static constexpr inline void _dotProduct(const Base::T_simd* a, const Base::T_simd* b, Base::T_data* result, std::index_sequence<Is...>) {
//         (([&]{
//             Base::T_simd temp = _mm256_mullo_epi16(a[Is], b[Is]);

//             __m128i hi = _mm256_extracti128_si256(temp, 1);
//             __m128i lo = _mm256_castsi256_si128(temp);
//             __m128i sum128 = _mm_add_epi16(lo, hi);

//             // Horizontal add to get the sum of all elements in the 128-bit lanes
//             sum128 = _mm_hadd_epi16(sum128, sum128);
//             sum128 = _mm_hadd_epi16(sum128, sum128);
//             sum128 = _mm_hadd_epi16(sum128, sum128);

//             *result += _mm_extract_epi16(sum128, 0);
//         }()), ...);
//     }

// };


#endif // STALKER_SIMD_MATHOPERATIONS_H

