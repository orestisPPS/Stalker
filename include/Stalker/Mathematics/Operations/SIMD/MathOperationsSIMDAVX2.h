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
        using Traits = TypeTraitsSIMD<double, T_SIMD::AVX2>;
        using T_simd = typename Traits::typeSIMD;
        using T_data = typename Traits::typeData;
        using Memory = MemoryOperationsSIMD<double, T_SIMD::AVX2>;
    private:

        friend Base;

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr) {

            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_fmadd_pd(_mm256_load_pd(a + _registerOffset<Is>()), *scalarA,
                                                                                       _mm256_mul_pd(_mm256_load_pd(b + _registerOffset<Is>()), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_pd(_mm256_load_pd(a + _registerOffset<Is>()),
                                                                                     _mm256_load_pd(b + _registerOffset<Is>()))), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _axpy(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>, const T_simd *scalar = nullptr) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_fmadd_pd(_mm256_load_pd(a + _registerOffset<Is>()), *scalar,
                                                                                   _mm256_load_pd(b + _registerOffset<Is>()))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                     const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr){
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_fmadd_pd(_mm256_load_pd(a + _registerOffset<Is>()), *scalarA,
                                                                                       _mm256_mul_pd(_mm256_load_pd(b + _registerOffset<Is>()), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_sub_pd(_mm256_load_pd(a + _registerOffset<Is>()),
                                                                                     _mm256_load_pd(b + _registerOffset<Is>()))), ...);

        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                     const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mul_pd(_mm256_mul_pd(_mm256_load_pd(a + _registerOffset<Is>()), *scalarA),
                                                                                     _mm256_mul_pd(_mm256_load_pd(b + _registerOffset<Is>()), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mul_pd(_mm256_load_pd(a + _registerOffset<Is>()),
                                                                                     _mm256_load_pd(b + _registerOffset<Is>()))), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mul_pd(_mm256_load_pd(data + _registerOffset<Is>()), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(data + _registerOffset<Is>(), _mm256_mul_pd(_mm256_load_pd(data + _registerOffset<Is>()), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_pd(_mm256_load_pd(data + _registerOffset<Is>()), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(data + _registerOffset<Is>(), _mm256_add_pd(_mm256_load_pd(data + _registerOffset<Is>()), *scalar)), ...);
        }
        
        template <size_t... Is>
        static inline void _sum(const T_data __restrict *data, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_pd(accumulators[Is], _mm256_load_pd(data + _registerOffset<Is>()))), ...);
        }

        template <size_t... Is>
        static inline void _dot(const T_data __restrict *a, const T_data __restrict *b, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_fmadd_pd(_mm256_load_pd(a + _registerOffset<Is>()), _mm256_load_pd(b + _registerOffset<Is>()), accumulators[Is])), ...);
        }
    };

    template <>
    struct MathOperationsSIMD<float,T_SIMD::AVX2>
            : public MathOperationsSIMDBase<float, T_SIMD::AVX2, MathOperationsSIMD<float,T_SIMD::AVX2>> {
        
        using Base = MathOperationsSIMDBase<float, T_SIMD::AVX2, MathOperationsSIMD<float,T_SIMD::AVX2>>;
        using Traits = TypeTraitsSIMD<float, T_SIMD::AVX2>;
        using T_simd = typename Traits::typeSIMD;
        using T_data = typename Traits::typeData;
        using Memory = MemoryOperationsSIMD<float, T_SIMD::AVX2>;

    private:
        
        friend Base;
        
        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_fmadd_ps(_mm256_load_ps(a + _registerOffset<Is>()), *scalarA,
                                                                                       _mm256_mul_ps(_mm256_load_ps(b + _registerOffset<Is>()), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_ps(_mm256_load_ps(a + _registerOffset<Is>()),
                                                                                     _mm256_load_ps(b + _registerOffset<Is>()))), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _axpy(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>, const T_simd *scalar = nullptr) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_fmadd_ps(_mm256_load_ps(a + _registerOffset<Is>()), *scalar,
                                                                                   _mm256_load_ps(b + _registerOffset<Is>()))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                     const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_fmadd_ps(_mm256_load_ps(a + _registerOffset<Is>()), *scalarA,
                                                                                       _mm256_mul_ps(_mm256_load_ps(b + _registerOffset<Is>()), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_sub_ps(_mm256_load_ps(a + _registerOffset<Is>()),
                                                                                     _mm256_load_ps(b + _registerOffset<Is>()))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                     const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mul_ps(_mm256_mul_ps(_mm256_load_ps(a + _registerOffset<Is>()), *scalarA),
                                                                                     _mm256_mul_ps(_mm256_load_ps(b + _registerOffset<Is>()), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mul_ps(_mm256_load_ps(a + _registerOffset<Is>()),
                                                                                     _mm256_load_ps(b + _registerOffset<Is>()))),...);
        }

        template <T_SIMDStore Policy,size_t... Is>
        static inline void _scale(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mul_ps(_mm256_load_ps(data + _registerOffset<Is>()), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(data + _registerOffset<Is>(), _mm256_mul_ps(_mm256_load_ps(data + _registerOffset<Is>()), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_ps(_mm256_load_ps(data + _registerOffset<Is>()), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(data + _registerOffset<Is>(), _mm256_add_ps(_mm256_load_ps(data + _registerOffset<Is>()), *scalar)), ...);
        }

        template <size_t... Is>
        static inline void _sum(const T_data* __restrict data, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_ps(_mm256_load_ps(data + _registerOffset<Is>()), accumulators[Is])), ...);
        }

        template <size_t... Is>
        static inline void _dot(const T_data __restrict *a, const T_data __restrict *b, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_fmadd_ps(_mm256_load_ps(a + _registerOffset<Is>()), _mm256_load_ps(b + _registerOffset<Is>()), accumulators[Is])), ...);
        }
    };

    template <>
    struct MathOperationsSIMD<int, T_SIMD::AVX2>
            : public MathOperationsSIMDBase<int, T_SIMD::AVX2, MathOperationsSIMD<int,T_SIMD::AVX2>> {
        
        using Base = MathOperationsSIMDBase<int, T_SIMD::AVX2, MathOperationsSIMD<int,T_SIMD::AVX2>>;
        using Traits = TypeTraitsSIMD<int, T_SIMD::AVX2>;
        using T_simd = typename Traits::typeSIMD;
        using T_data = typename Traits::typeData;
        using Memory = MemoryOperationsSIMD<int, T_SIMD::AVX2>;

    private:
        
        friend Base;

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi32(
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())), *scalarA),
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi32(
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))),...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _axpy(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>, const T_simd *scalar = nullptr) {
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi32(_mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())), *scalar),
                                                                                                           _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                     const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_sub_epi32(
                            _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + _registerOffset<Is>())), *scalarA),
                            _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(b + _registerOffset<Is>())), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_sub_epi32(
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                     const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mullo_epi32(
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())), *scalarA),
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mullo_epi32(
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }
        
        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(data + _registerOffset<Is>(), _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(data + _registerOffset<Is>(), _mm256_add_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }

        template <size_t... Is>
        static inline void _sum(const T_data __restrict *data, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_epi32(accumulators[Is], _mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())))), ...);
        }

        template <size_t... Is>
        static inline void _dot(const T_data __restrict *a, const T_data __restrict *b, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_epi32(accumulators[Is],_mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                                                      _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>()))))), ...);
        }
    };

    template <>
    struct MathOperationsSIMD<unsigned int,T_SIMD::AVX2>
            : public MathOperationsSIMDBase<unsigned int, T_SIMD::AVX2, MathOperationsSIMD<unsigned int,T_SIMD::AVX2>> {
        
        using Base = MathOperationsSIMDBase<unsigned int, T_SIMD::AVX2, MathOperationsSIMD<unsigned int,T_SIMD::AVX2>>;
        using Traits = TypeTraitsSIMD<unsigned int, T_SIMD::AVX2>;
        using T_simd = typename Traits::typeSIMD;
        using T_data = typename Traits::typeData;
        using Memory = MemoryOperationsSIMD<unsigned int, T_SIMD::AVX2>;

    private:

        friend Base;


        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi32(
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())), *scalarA),
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi32(
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))),...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _axpy(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>, const T_simd *scalar = nullptr) {
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi32(_mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())), *scalar),
                                                                                                                      _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                     const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_sub_epi32(
                            _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + _registerOffset<Is>())), *scalarA),
                            _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(b + _registerOffset<Is>())), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_sub_epi32(
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                     const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mullo_epi32(
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())), *scalarA),
                                                  _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mullo_epi32(
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(data + _registerOffset<Is>(), _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(data + _registerOffset<Is>(), _mm256_add_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }

        template <size_t... Is>
        static inline void _sum(const T_data __restrict *data, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_epi32(accumulators[Is], _mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())))), ...);
        }

        template <size_t... Is>
        static inline void _dot(const T_data __restrict *a, const T_data __restrict *b, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_epi32(accumulators[Is],_mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                                                      _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>()))))), ...);
        }
    };

    template <>
    struct MathOperationsSIMD<short,T_SIMD::AVX2>
            : public MathOperationsSIMDBase<short, T_SIMD::AVX2, MathOperationsSIMD<short,T_SIMD::AVX2>> {
        
        using Base = MathOperationsSIMDBase<short, T_SIMD::AVX2, MathOperationsSIMD<short,T_SIMD::AVX2>>;
        using Traits = TypeTraitsSIMD<short, T_SIMD::AVX2>;
        using T_simd = typename Traits::typeSIMD;
        using T_data = typename Traits::typeData;
        using Memory = MemoryOperationsSIMD<short, T_SIMD::AVX2>;

    private:

        friend Base;

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi16(
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())), *scalarA),
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi16(
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _axpy(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>, const T_simd *scalar = nullptr) {
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi16(_mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())), *scalar),
                                                                                                           _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                     const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_sub_epi16(
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())), *scalarA),
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())), *scalarB))), ...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_sub_epi16(
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                     const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mullo_epi16(
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())), *scalarA),
                                                  _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())), *scalarB))),...);
            else
                (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mullo_epi16(
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                  _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>())))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _scale(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(data + _registerOffset<Is>(), _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(result + _registerOffset<Is>(), _mm256_add_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
            (Memory::store<Policy>(data + _registerOffset<Is>(), _mm256_add_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())), *scalar)), ...);
        }

        template <size_t... Is>
        static inline void _sum(const T_data __restrict *data, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_epi16(accumulators[Is], _mm256_load_si256(reinterpret_cast<const __m256i *>(data + _registerOffset<Is>())))), ...);
        }

        template <size_t... Is>
        static inline void _dot(const T_data __restrict *a, const T_data __restrict *b, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm256_add_epi16(accumulators[Is],_mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i *>(a + _registerOffset<Is>())),
                                                                                      _mm256_load_si256(reinterpret_cast<const __m256i *>(b + _registerOffset<Is>()))))), ...);
        }
    };

} // namespace Stalker::Mathematics::SIMD
