#pragma once

#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDAVX2.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDBase.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX2.h>
#include <Stalker/Core/Config/SIMD.h>

namespace Stalker::Mathematics::SIMD
{
    // using namespace Stalker::Core::Config;

    template <>
    struct MathOperationsSIMD<double,T_SIMD::AVX2>
            : public MathOperationsSIMDBase<double, T_SIMD::AVX2, MathOperationsSIMD<double,T_SIMD::AVX2>> {
        using Base = MathOperationsSIMDBase<double, T_SIMD::AVX2, MathOperationsSIMD<double,T_SIMD::AVX2>>;
        
        private:

        friend Base;

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {

            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_fmadd_pd(_mm256_load_pd(a + Is * Base::registerSize), *scalarA,
                                                                                                  _mm256_mul_pd(_mm256_load_pd(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_pd(_mm256_load_pd(a + Is * Base::registerSize),
                                                                                                _mm256_load_pd(b + Is * Base::registerSize))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr){
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_fmadd_pd(_mm256_load_pd(a + Is * Base::registerSize), *scalarA,
                                                                                                  _mm256_mul_pd(_mm256_load_pd(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_pd(_mm256_load_pd(a + Is * Base::registerSize),
                                                                                                _mm256_load_pd(b + Is * Base::registerSize))), ...);

        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mul_pd(_mm256_mul_pd(_mm256_load_pd(a + Is * Base::registerSize), *scalarA),
                                                                                                _mm256_mul_pd(_mm256_load_pd(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mul_pd(_mm256_load_pd(a + Is * Base::registerSize),
                                                                                                _mm256_load_pd(b + Is * Base::registerSize))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _scale(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mul_pd(_mm256_load_pd(data + Is * Base::registerSize), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm256_mul_pd(_mm256_load_pd(data + Is * Base::registerSize), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_pd(_mm256_load_pd(data + Is * Base::registerSize), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm256_add_pd(_mm256_load_pd(data + Is * Base::registerSize), *scalar)), ...);
        }
        
        template <size_t... Is>
        static inline void _sum(const Base::T_data __restrict *data, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_pd(accumulators[Is], _mm256_loadu_pd(data + Is * Base::registerSize))), ...);
        }

        static inline T_data _horizontalRegisterSum(const Base::T_simd* __restrict data) {
            __m128d low = _mm256_castpd256_pd128(*data);
            __m128d high = _mm256_extractf128_pd(*data, 1);
            __m128d sum128 = _mm_add_pd(low, high);
            __m128d swapped = _mm_shuffle_pd(sum128, sum128, 1);
            return _mm_cvtsd_f64(_mm_add_sd(sum128, swapped));
        }
    };

    template <>
    struct MathOperationsSIMD<float,T_SIMD::AVX2>
            : public MathOperationsSIMDBase<float, T_SIMD::AVX2, MathOperationsSIMD<float,T_SIMD::AVX2>> {
        using Base = MathOperationsSIMDBase<float, T_SIMD::AVX2, MathOperationsSIMD<float,T_SIMD::AVX2>>;
        
        private:
        
        friend Base;
        
        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_fmadd_ps(_mm256_load_ps(a + Is * Base::registerSize), *scalarA,
                                                                                                  _mm256_mul_ps(_mm256_load_ps(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_ps(_mm256_load_ps(a + Is * Base::registerSize),
                                                                                                _mm256_load_ps(b + Is * Base::registerSize))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_fmadd_ps(_mm256_load_ps(a + Is * Base::registerSize), *scalarA,
                                                                                                _mm256_mul_ps(_mm256_load_ps(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_ps(_mm256_load_ps(a + Is * Base::registerSize),
                                                                                                _mm256_load_ps(b + Is * Base::registerSize))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mul_ps(_mm256_mul_ps(_mm256_load_ps(a + Is * Base::registerSize), *scalarA),
                                                                                                _mm256_mul_ps(_mm256_load_ps(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mul_ps(_mm256_load_ps(a + Is * Base::registerSize),
                                                                                                _mm256_load_ps(b + Is * Base::registerSize))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _scale(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mul_ps(_mm256_load_ps(data + Is * Base::registerSize), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm256_mul_ps(_mm256_load_ps(data + Is * Base::registerSize), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_ps(_mm256_load_ps(data + Is * Base::registerSize), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm256_add_ps(_mm256_load_ps(data + Is * Base::registerSize), *scalar)), ...);
        }

        template <size_t... Is>
        static inline void _sum(const Base::T_data* __restrict data, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_ps(accumulators[Is], _mm256_load_ps(data + Is * Base::registerSize))), ...);
        }

static inline float _horizontalRegisterSum(const __m256* __restrict data) {
    // Debug version - store and sum manually to verify
    alignas(32) float temp[8];
    _mm256_store_ps(temp, *data);
    
    float sum = 0.0f;
    for (int i = 0; i < 8; i++) {
        sum += temp[i];
    }
    return sum;
}

    };

    template <>
    struct MathOperationsSIMD<int, T_SIMD::AVX2>
            : public MathOperationsSIMDBase<int, T_SIMD::AVX2, MathOperationsSIMD<int,T_SIMD::AVX2>> {
        using Base = MathOperationsSIMDBase<int, T_SIMD::AVX2, MathOperationsSIMD<int,T_SIMD::AVX2>>;
        
        private:
        
        friend Base;

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_epi32(
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)), *scalarA),
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_epi32(
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)),
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_epi32(
                            _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)), *scalarA),
                            _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_epi32(
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)),
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mullo_epi32(
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)), *scalarA),
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mullo_epi32(
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)),
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _scale(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }
        
        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm256_add_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <size_t... Is>
        static inline void _sum(const Base::T_data __restrict *data, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_epi32(accumulators[Is], _mm256_loadu_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)))), ...);
        }

        static inline int _horizontalRegisterSum(const Base::T_simd* __restrict data) {
            // Sum 128-bit lanes
            __m128i low = _mm256_castsi256_si128(*data);
            __m128i high = _mm256_extracti128_si256(*data, 1);
            __m128i sum128 = _mm_add_epi32(low, high);
            
            // Efficient 4-element reduction
            __m128i hi64 = _mm_unpackhi_epi64(sum128, sum128);  // Copy high 64 bits to low
            __m128i sum64 = _mm_add_epi32(sum128, hi64);        // [a0+a2, a1+a3, ...]
            __m128i hi32 = _mm_shuffle_epi32(sum64, 0b11110101);  // [a1+a3, ...]
            return _mm_extract_epi32(_mm_add_epi32(sum64, hi32), 0);
        }

    };

    template <>
    struct MathOperationsSIMD<unsigned int,T_SIMD::AVX2>
            : public MathOperationsSIMDBase<unsigned int, T_SIMD::AVX2, MathOperationsSIMD<unsigned int,T_SIMD::AVX2>> {
        using Base = MathOperationsSIMDBase<unsigned int, T_SIMD::AVX2, MathOperationsSIMD<unsigned int,T_SIMD::AVX2>>;
        
        private:

        friend Base;


        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_epi32(
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)), *scalarA),
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_epi32(
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)),
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_epi32(
                            _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)), *scalarA),
                            _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_epi32(
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)),
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mullo_epi32(
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)), *scalarA),
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mullo_epi32(
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)),
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _scale(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm256_add_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <size_t... Is>
        static inline void _sum(const Base::T_data __restrict *data, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_epi32(accumulators[Is], _mm256_loadu_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)))), ...);
        }

        static inline T_data _horizontalRegisterSum(const Base::T_simd* __restrict data) {
            // Same as signed int - bitwise identical operations
            __m128i low = _mm256_castsi256_si128(*data);
            __m128i high = _mm256_extracti128_si256(*data, 1);
            __m128i sum128 = _mm_add_epi32(low, high);
            
            __m128i hi64 = _mm_unpackhi_epi64(sum128, sum128);
            __m128i sum64 = _mm_add_epi32(sum128, hi64);
            __m128i hi32 = _mm_shuffle_epi32(sum64, 0b11110101);
            return (T_data)_mm_extract_epi32(_mm_add_epi32(sum64, hi32), 0);
        }
    };

    template <>
    struct MathOperationsSIMD<short,T_SIMD::AVX2>
            : public MathOperationsSIMDBase<short, T_SIMD::AVX2, MathOperationsSIMD<short,T_SIMD::AVX2>> {
        using Base = MathOperationsSIMDBase<short, T_SIMD::AVX2, MathOperationsSIMD<short,T_SIMD::AVX2>>;
        
    private:

        friend Base;

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_epi16(
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)), *scalarA),
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_epi16(
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)),
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_epi16(
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)), *scalarA),
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_epi16(
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)),
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mullo_epi16(
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)), *scalarA),
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)), *scalarB))),...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mullo_epi16(
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)),
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _scale(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm256_add_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <size_t... Is>
        static inline void _sum(const Base::T_data __restrict *data, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_epi16(accumulators[Is], _mm256_loadu_si256(reinterpret_cast<const __m256i *>(data + Is * Base::registerSize)))), ...);
        }

        static inline T_data _horizontalRegisterSum(const Base::T_simd* __restrict data) {
            // Sum to 32-bit to prevent overflow
            __m256i evens = _mm256_srli_epi32(*data, 16);     // Shift high 16-bits to low
            __m256i sum32 = _mm256_add_epi32(*data, evens);   // Add low and high 16-bits

            // Now reduce 8x 32-bit integers
            __m128i low = _mm256_castsi256_si128(sum32);
            __m128i high = _mm256_extracti128_si256(sum32, 1);
            __m128i sum128 = _mm_add_epi32(low, high);
            
            // Horizontal reduction
            __m128i hi64 = _mm_unpackhi_epi64(sum128, sum128);
            __m128i sum64 = _mm_add_epi32(sum128, hi64);
            __m128i hi32 = _mm_shuffle_epi32(sum64, 0b11110101);
            return _mm_extract_epi32(_mm_add_epi32(sum64, hi32), 0);
        }
    };

} // namespace Stalker::Mathematics::SIMD
