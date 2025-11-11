#pragma once

#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDAVX512.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDBase.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX512.h>

namespace Stalker::Mathematics::SIMD {

template<>
struct MathOperationsSIMD<double, T_SIMD::AVX512>
        : public MathOperationsSIMDBase<double, T_SIMD::AVX512, MathOperationsSIMD<double, T_SIMD::AVX512>> {
    using Base = MathOperationsSIMDBase<double, T_SIMD::AVX512, MathOperationsSIMD<double, T_SIMD::AVX512>>;

private:
    
    friend Base;

    template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                            const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {

        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_fmadd_pd(_mm512_load_pd(a + Is * Base::registerSize), *scalarA,
                                                                                              _mm512_mul_pd(_mm512_load_pd(b + Is * Base::registerSize), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_pd(_mm512_load_pd(a + Is * Base::registerSize),
                                                                                            _mm512_load_pd(b + Is * Base::registerSize))), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _axpy(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>, const Base::T_simd *scalar = nullptr) {
        (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_fmadd_pd(_mm512_load_pd(a + Is * Base::registerSize), *scalar,
                                                                                          _mm512_load_pd(b + Is * Base::registerSize))), ...);
    }

    template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr){
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_fmadd_pd(_mm512_load_pd(a + Is * Base::registerSize), *scalarA,
                                                                                              _mm512_mul_pd(_mm512_load_pd(b + Is * Base::registerSize), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_pd(_mm512_load_pd(a + Is * Base::registerSize),
                                                                                            _mm512_load_pd(b + Is * Base::registerSize))), ...);

    }

    // (multiply implementation stays as-is; no fma needed)
    template<T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result,
                                 std::index_sequence<Is...>,
                                 const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mul_pd(_mm512_mul_pd(_mm512_load_pd(a + Is * Base::registerSize), *scalarA),
                                                                                            _mm512_mul_pd(_mm512_load_pd(b + Is * Base::registerSize), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mul_pd(_mm512_load_pd(a + Is * Base::registerSize),
                                                                                            _mm512_load_pd(b + Is * Base::registerSize))), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _scale(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mul_pd(_mm512_load_pd(data + Is * Base::registerSize), *scalar)), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _scale(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm512_mul_pd(_mm512_load_pd(data + Is * Base::registerSize), *scalar)), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_pd(_mm512_load_pd(data + Is * Base::registerSize), *scalar)), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm512_add_pd(_mm512_load_pd(data + Is * Base::registerSize), *scalar)), ...);
    }

    template <size_t... Is>
    static inline void _sum(const Base::T_data* __restrict data, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_add_pd(accumulators[Is], _mm512_load_pd(data + Is * Base::registerSize))), ...);
    }

    template <size_t... Is>
    static inline void _dot(const Base::T_data __restrict *a, const Base::T_data __restrict *b, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_fmadd_pd(_mm512_load_pd(a + Is * Base::registerSize), _mm512_load_pd(b + Is * Base::registerSize), accumulators[Is])), ...);
    }
};

template<>
struct MathOperationsSIMD<float, T_SIMD::AVX512>
        : public MathOperationsSIMDBase<float, T_SIMD::AVX512, MathOperationsSIMD<float, T_SIMD::AVX512>> {
    using Base = MathOperationsSIMDBase<float, T_SIMD::AVX512, MathOperationsSIMD<float, T_SIMD::AVX512>>;
    
private:
    
    friend Base;

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_fmadd_ps(_mm512_load_ps(a + Is * Base::registerSize), *scalarA,
                                                                                                  _mm512_mul_ps(_mm512_load_ps(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_ps(_mm512_load_ps(a + Is * Base::registerSize),
                                                                                                _mm512_load_ps(b + Is * Base::registerSize))), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _axpy(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>, const Base::T_simd *scalar = nullptr) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_fmadd_ps(_mm512_load_ps(a + Is * Base::registerSize), *scalar,
                                                                                            _mm512_load_ps(b + Is * Base::registerSize))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_fmadd_ps(_mm512_load_ps(a + Is * Base::registerSize), *scalarA,
                                                                                                  _mm512_mul_ps(_mm512_load_ps(b + Is * Base::registerSize), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_ps(_mm512_load_ps(a + Is * Base::registerSize),
                                                                                                _mm512_load_ps(b + Is * Base::registerSize))),...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mul_ps(_mm512_mul_ps(_mm512_load_ps(a + Is * Base::registerSize), *scalarA),
                                                                                                _mm512_mul_ps(_mm512_load_ps(b + Is * Base::registerSize), *scalarB))),...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mul_ps(_mm512_load_ps(a + Is * Base::registerSize),
                                                                                                _mm512_load_ps(b + Is * Base::registerSize))),...);
        }

        template <T_SIMDStore Policy,size_t... Is>
        static inline void _scale(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mul_ps(_mm512_load_ps(data + Is * Base::registerSize), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm512_mul_ps(_mm512_load_ps(data + Is * Base::registerSize), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_ps(_mm512_load_ps(data + Is * Base::registerSize), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm512_add_ps(_mm512_load_ps(data + Is * Base::registerSize), *scalar)), ...);
        }

        template <size_t... Is>
        static inline void _sum(const Base::T_data* __restrict data, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm512_add_ps(accumulators[Is], _mm512_load_ps(data + Is * Base::registerSize))), ...);
        }

        template <size_t... Is>
        static inline void _dot(const Base::T_data __restrict *a, const Base::T_data __restrict *b, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm512_fmadd_ps(_mm512_load_ps(a + Is * Base::registerSize), _mm512_load_ps(b + Is * Base::registerSize), accumulators[Is])), ...);
        }
};

template<>
struct MathOperationsSIMD<int, T_SIMD::AVX512>
        : public MathOperationsSIMDBase<int, T_SIMD::AVX512, MathOperationsSIMD<int, T_SIMD::AVX512>> {
    using Base = MathOperationsSIMDBase<int, T_SIMD::AVX512, MathOperationsSIMD<int, T_SIMD::AVX512>>;
    
private:
    
    friend Base;

    template<T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                                                                                               _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB)) ), ...);
        }
        else {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                          _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<T_SIMDStore Policy, size_t... Is>
    static inline void _axpy(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalar = nullptr) {
        (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalar),
                                                                                                              _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }

    template<T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(
                result + Is * Base::registerSize, _mm512_sub_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                                                                   _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                               _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }

    template<T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(
                result + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                                                                     _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                                 _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _scale(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
    }
    
    template <T_SIMDStore Policy, size_t... Is>
    static inline void _scale(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm512_add_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
    }

    template <size_t... Is>
    static inline void _sum(const Base::T_data* __restrict data, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_add_epi32(accumulators[Is], _mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)))), ...);
    }
    
    template <size_t... Is>
    static inline void _dot(const Base::T_data __restrict *a, const Base::T_data __restrict *b, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_add_epi32(accumulators[Is],_mm512_mullo_epi32(_mm512_loadu_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)),
                                                                                    _mm512_loadu_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize))))), ...);
    }
};

template<>
struct MathOperationsSIMD<unsigned int, T_SIMD::AVX512>
        : public MathOperationsSIMDBase<unsigned int, T_SIMD::AVX512, MathOperationsSIMD<unsigned int, T_SIMD::AVX512>> {
    using Base = MathOperationsSIMDBase<unsigned int, T_SIMD::AVX512, MathOperationsSIMD<unsigned int, T_SIMD::AVX512>>;
    
private:

friend Base;

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr){
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)), *scalarA),
                                                                                                   _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)),
                                                                                                   _mm512_load_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize)))), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _axpy(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>, const Base::T_simd *scalar = nullptr) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)), *scalar),
                                                                                                                  _mm512_load_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize)))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                         const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr){
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)), *scalarA),
                                                                                                   _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)),
                                                                                                   _mm512_load_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize)))), ...);
        }

        template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                     const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr)
        {
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)), *scalarA),
                                                                                                     _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize)), *scalarB))),...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)), 
                                                                                                     _mm512_load_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize)))),...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
            (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm512_add_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
        }

        template <size_t... Is>
        static inline void _sum(const Base::T_data* __restrict data, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm512_add_epi32(accumulators[Is], _mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)))), ...);
        }

        template <size_t... Is>
        static inline void _dot(const Base::T_data __restrict *a, const Base::T_data __restrict *b, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm512_add_epi32(accumulators[Is],_mm512_mullo_epi32(_mm512_loadu_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)),
                                                                                      _mm512_loadu_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize))))), ...);
        }
    };

template<>
struct MathOperationsSIMD<short, T_SIMD::AVX512>
        : public MathOperationsSIMDBase<short, T_SIMD::AVX512, MathOperationsSIMD<short, T_SIMD::AVX512>> {
    using Base = MathOperationsSIMDBase<short, T_SIMD::AVX512, MathOperationsSIMD<short, T_SIMD::AVX512>>;
    
private:
    
    friend Base;

    template<T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize,_mm512_add_epi16(_mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                                                                                              _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                               _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }

    template<T_SIMDStore Policy, size_t... Is>
    static inline void _axpy(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalar = nullptr) {
        (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi16(_mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalar),
                                                                                                              _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }

    template<T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_epi16(_mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                                                                                               _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                               _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }

    template<T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(
                result + Is * Base::registerSize, _mm512_mullo_epi16(_mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                                                                     _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                                 _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }

    template <T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _scale(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _scale(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(const Base::T_data *data, Base::T_data *result, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi16(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(__restrict Base::T_data *data, const Base::T_simd *scalar, std::index_sequence<Is...>) {
        (Base::MemoryOps::store<Policy>(data + Is * Base::registerSize, _mm512_add_epi16(_mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)), *scalar)), ...);
    }

    template <size_t... Is>
    static inline void _sum(const Base::T_data* __restrict data, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_add_epi16(accumulators[Is], _mm512_load_si512(reinterpret_cast<const __m512i *>(data + Is * Base::registerSize)))), ...);
    }

    template <size_t... Is>
    static inline void _dot(const Base::T_data __restrict *a, const Base::T_data __restrict *b, Base::T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_add_epi16(accumulators[Is],_mm512_mullo_epi16(_mm512_loadu_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)),
                                                                                  _mm512_loadu_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize))))), ...);
    }
};



} // namespace Stalker::Mathematics::SIMD

