#pragma once

#include <immintrin.h> // <-- Required for AVX-512 intrinsics
#include <Stalker/Core/Traits/SIMD/SIMDTraitsAVX512.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDBase.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX512.h>

namespace Stalker::Mathematics::SIMD {

template<>
struct SIMDMathOperations<double, AVX512>
        : public SIMDMathOperationsBase<double, AVX512, SIMDMathOperations<double, AVX512>> {
    using Base = SIMDMathOperationsBase<double, AVX512, SIMDMathOperations<double, AVX512>>;

private:
    
    friend Base;

    template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                            const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr) {

        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_fmadd_pd(_mm512_load_pd(a + Is * Base::registerSize), *scalarA,
                                                                                              _mm512_mul_pd(_mm512_load_pd(b + Is * Base::registerSize), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_pd(_mm512_load_pd(a + Is * Base::registerSize),
                                                                                            _mm512_load_pd(b + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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
    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
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
};

template<>
struct SIMDMathOperations<float, AVX512>
        : public SIMDMathOperationsBase<float, AVX512, SIMDMathOperations<float, AVX512>> {
    using Base = SIMDMathOperationsBase<float, AVX512, SIMDMathOperations<float, AVX512>>;
    
private:
    
    friend Base;

template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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
};

template<>
struct SIMDMathOperations<int, AVX512>
        : public SIMDMathOperationsBase<int, AVX512, SIMDMathOperations<int, AVX512>> {
    using Base = SIMDMathOperationsBase<int, AVX512, SIMDMathOperations<int, AVX512>>;
    
private:
    
    friend Base;

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
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

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(
                result + Is * Base::registerSize, _mm512_sub_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                                                                   _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                               _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(
                result + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                                                                     _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                                 _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }
};

template<>
struct SIMDMathOperations<unsigned int, AVX512>
        : public SIMDMathOperationsBase<unsigned int, AVX512, SIMDMathOperations<unsigned int, AVX512>> {
    using Base = SIMDMathOperationsBase<unsigned int, AVX512, SIMDMathOperations<unsigned int, AVX512>>;
    
private:

friend Base;

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
        static inline void _add(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                                const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr){
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)), *scalarA),
                                                                                                   _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)),
                                                                                                   _mm512_load_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize)))), ...);
        }

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const Base::T_data *a, const Base::T_data *b, Base::T_data *result, std::index_sequence<Is...>,
                         const Base::T_simd *scalarA = nullptr, const Base::T_simd *scalarB = nullptr){
            if constexpr (IsScaled)
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_epi32(_mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)), *scalarA),
                                                                                                   _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize)), *scalarB))), ...);
            else
                (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_epi32(_mm512_load_si512(reinterpret_cast<const __m512i *>(a + Is * Base::registerSize)),
                                                                                                   _mm512_load_si512(reinterpret_cast<const __m512i *>(b + Is * Base::registerSize)))), ...);
        }

        template <SIMDStoreType Policy, bool IsScaled, size_t... Is>
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
    };

template<>
struct SIMDMathOperations<short, AVX512>
        : public SIMDMathOperationsBase<short, AVX512, SIMDMathOperations<short, AVX512>> {
    using Base = SIMDMathOperationsBase<short, AVX512, SIMDMathOperations<short, AVX512>>;
    
private:
    
    friend Base;

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize,_mm512_add_epi16(_mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                                                                                              _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_add_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                               _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }   

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_epi16(_mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                                                                                               _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_sub_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                               _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Base::MemoryOps::store<Policy>(
                result + Is * Base::registerSize, _mm512_mullo_epi16(_mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                                                                     _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB))), ...);
        else
            (Base::MemoryOps::store<Policy>(result + Is * Base::registerSize, _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                                 _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
    }
};



} // namespace Stalker::Mathematics::SIMD

