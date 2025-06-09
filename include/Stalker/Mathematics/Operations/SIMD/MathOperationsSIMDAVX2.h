#pragma once

#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDAVX2.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDBase.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX2.h>

namespace Stalker::Mathematics::SIMD
{
    template <>
    struct SIMDMathOperations<double, AVX2>
            : public SIMDMathOperationsBase<double, AVX2, SIMDMathOperations<double, AVX2>> {
        using Base = SIMDMathOperationsBase<double, AVX2, SIMDMathOperations<double, AVX2>>;
        
        private:

        friend Base;

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
        static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {

            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_fmadd_pd(_mm256_load_pd(a + Is * Base::registerSize), *scalarA,
                                                                                                _mm256_mul_pd(_mm256_load_pd(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_pd(_mm256_load_pd(a + Is * Base::registerSize),
                                                                                              _mm256_load_pd(b + Is * Base::registerSize))), ...);
        }

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr){
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_fmadd_pd(_mm256_load_pd(a + Is * Base::registerSize), *scalarA,
                                                                                                _mm256_mul_pd(_mm256_load_pd(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_pd(_mm256_load_pd(a + Is * Base::registerSize),
                                                                                              _mm256_load_pd(b + Is * Base::registerSize))), ...);

        }

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mul_pd(_mm256_load_pd(a + Is * Base::registerSize),
                                                                                              _mm256_load_pd(b + Is * Base::registerSize))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mul_pd(_mm256_load_pd(a + Is * Base::registerSize),
                                                                                              _mm256_load_pd(b + Is * Base::registerSize))), ...);
        }
    };

    template <>
    struct SIMDMathOperations<float, AVX2>
            : public SIMDMathOperationsBase<float, AVX2, SIMDMathOperations<float, AVX2>> {
        using Base = SIMDMathOperationsBase<float, AVX2, SIMDMathOperations<float, AVX2>>;
        
        private:
        
        friend Base;
        
        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
        static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_fmadd_ps(_mm256_load_ps(a + Is * Base::registerSize), *scalarA,
                                                                                                _mm256_mul_ps(_mm256_load_ps(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_add_ps(_mm256_load_ps(a + Is * Base::registerSize),
                                                                                              _mm256_load_ps(b + Is * Base::registerSize))), ...);
        }

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_fmadd_ps(_mm256_load_ps(a + Is * Base::registerSize), *scalarA,
                                                                                                _mm256_mul_ps(_mm256_load_ps(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_ps(_mm256_load_ps(a + Is * Base::registerSize),
                                                                                              _mm256_load_ps(b + Is * Base::registerSize))),...);
        }

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mul_ps(_mm256_mul_ps(_mm256_load_ps(a + Is * Base::registerSize), *scalarA),
                                                                                              _mm256_mul_ps(_mm256_load_ps(b + Is * Base::registerSize), *scalarB))),
                 ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mul_ps(_mm256_load_ps(a + Is * Base::registerSize),
                                                                                              _mm256_load_ps(b + Is * Base::registerSize))),...);
        }
    };

    template <>
    struct SIMDMathOperations<int, AVX2>
            : public SIMDMathOperationsBase<int, AVX2, SIMDMathOperations<int, AVX2>> {
        using Base = SIMDMathOperationsBase<int, AVX2, SIMDMathOperations<int, AVX2>>;
        
        private:
        
        friend Base;

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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
    };

    template <>
    struct SIMDMathOperations<unsigned int, AVX2>
            : public SIMDMathOperationsBase<unsigned int, AVX2, SIMDMathOperations<unsigned int, AVX2>> {
        using Base = SIMDMathOperationsBase<unsigned int, AVX2, SIMDMathOperations<unsigned int, AVX2>>;
        
        private:

        friend Base;


        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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
    };

    template <>
    struct SIMDMathOperations<short, AVX2>
            : public SIMDMathOperationsBase<short, AVX2, SIMDMathOperations<short, AVX2>> {
        using Base = SIMDMathOperationsBase<short, AVX2, SIMDMathOperations<short, AVX2>>;
        
    private:

        friend Base;

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_epi16(
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)), *scalarA),
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_sub_epi16( _mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)),
                                                                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)))), ...);
        }

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm256_mullo_epi16(
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)), *scalarA),
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)), *scalarB))),
                 ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize,
                                              _mm256_mullo_epi16(
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(a + Is * Base::registerSize)),
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + Is * Base::registerSize)))),
                 ...);
        }
    };

} // namespace Stalker::Mathematics::SIMD
