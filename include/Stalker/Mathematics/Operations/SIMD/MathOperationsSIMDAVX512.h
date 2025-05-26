#pragma once

#include <immintrin.h> // <-- Required for AVX-512 intrinsics
#include <Stalker/Core/Traits/SIMD/SIMDTraitsAVX512.h>
#include <Stalker/Mathematics/Operations/MathOperationsSIMD.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX512.h>

namespace Stalker::Mathematics::SIMD {

using namespace Stalker::Memory::SIMD;
using namespace Stalker::Core;

template<>
struct SIMDMathOperations<double, SIMDType::AVX512>
    : public SIMDMathOperationsBase<double, SIMDType::AVX512, SIMDMathOperations<double, SIMDType::AVX512>> {

    using Base = SIMDMathOperationsBase<double, SIMDType::AVX512, SIMDMathOperations<double, SIMDType::AVX512>>;
    friend Base;
private:

    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm512_store_pd(destination, source);
            else{
                _mm512_stream_pd(destination, source);
            }
        }
    };

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_add_pd(
                    _mm512_mul_pd(_mm512_load_pd(a + Is * Base::registerSize), *scalarA),
                    _mm512_mul_pd(_mm512_load_pd(b + Is * Base::registerSize), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_add_pd(_mm512_load_pd(a + Is * Base::registerSize),
                                                                                          _mm512_load_pd(b + Is * Base::registerSize))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_sub_pd(
                    _mm512_mul_pd(_mm512_load_pd(a + Is * Base::registerSize), *scalarA),
                    _mm512_mul_pd(_mm512_load_pd(b + Is * Base::registerSize), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_sub_pd(_mm512_load_pd(a + Is * Base::registerSize),
                                                                                          _mm512_load_pd(b + Is * Base::registerSize))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_mul_pd(
                    _mm512_mul_pd(_mm512_load_pd(a + Is * Base::registerSize), *scalarA),
                    _mm512_mul_pd(_mm512_load_pd(b + Is * Base::registerSize), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_mul_pd(_mm512_load_pd(a + Is * Base::registerSize),
                                                                                          _mm512_load_pd(b + Is * Base::registerSize))), ...);
        }
    }
};

template<>
struct SIMDMathOperations<float, SIMDType::AVX512>
    : public SIMDMathOperationsBase<float, SIMDType::AVX512, SIMDMathOperations<float, SIMDType::AVX512>> {

    using Base = SIMDMathOperationsBase<float, SIMDType::AVX512, SIMDMathOperations<float, SIMDType::AVX512>>;
    friend Base;
private:
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm512_store_ps(destination, source);
            else{
                _mm512_stream_ps(destination, source);
            }
        }
    };

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_add_ps(
                    _mm512_mul_ps(_mm512_load_ps(a + Is * Base::registerSize), *scalarA),
                    _mm512_mul_ps(_mm512_load_ps(b + Is * Base::registerSize), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_add_ps(_mm512_load_ps(a + Is * Base::registerSize),
                                                                                          _mm512_load_ps(b + Is * Base::registerSize))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_sub_ps(
                    _mm512_mul_ps(_mm512_load_ps(a + Is * Base::registerSize), *scalarA),
                    _mm512_mul_ps(_mm512_load_ps(b + Is * Base::registerSize), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_sub_ps(_mm512_load_ps(a + Is * Base::registerSize),
                                                                                          _mm512_load_ps(b + Is * Base::registerSize))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_mul_ps(
                    _mm512_mul_ps(_mm512_load_ps(a + Is * Base::registerSize), *scalarA),
                    _mm512_mul_ps(_mm512_load_ps(b + Is * Base::registerSize), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_mul_ps(_mm512_load_ps(a + Is * Base::registerSize),
                                                                                          _mm512_load_ps(b + Is * Base::registerSize))), ...);
        }
    }
};

template<>
struct SIMDMathOperations<int, SIMDType::AVX512>
    : public SIMDMathOperationsBase<int, SIMDType::AVX512, SIMDMathOperations<int, SIMDType::AVX512>> {

    using Base = SIMDMathOperationsBase<int, SIMDType::AVX512, SIMDMathOperations<int, SIMDType::AVX512>>;
    friend Base;
private:
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm512_store_si512(reinterpret_cast<void*>(destination), source);
            else{
                _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
            }
        }
    };

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_add_epi32(
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                          _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_sub_epi32(
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_sub_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                          _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_mullo_epi32(
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                          _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
        }
    }
};

template<>
struct SIMDMathOperations<unsigned int, SIMDType::AVX512>
    : public SIMDMathOperationsBase<unsigned int, SIMDType::AVX512, SIMDMathOperations<unsigned int, SIMDType::AVX512>> {

    using Base = SIMDMathOperationsBase<unsigned int, SIMDType::AVX512, SIMDMathOperations<unsigned int, SIMDType::AVX512>>;
    friend Base;
private:
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm512_store_si512(reinterpret_cast<void*>(destination), source);
            else{
                _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
            }
        }
    };

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_add_epi32(
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_add_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                          _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_sub_epi32(
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_sub_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                             _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
        }
    }
    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_mullo_epi32(
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                    _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_mullo_epi32(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                             _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
        }
    }
};

template<>
struct SIMDMathOperations<short, SIMDType::AVX512>
    : public SIMDMathOperationsBase<short, SIMDType::AVX512, SIMDMathOperations<short, SIMDType::AVX512>> {

    using Base = SIMDMathOperationsBase<short, SIMDType::AVX512, SIMDMathOperations<short, SIMDType::AVX512>>;
    friend Base;
private:
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm512_store_si512(reinterpret_cast<void*>(destination), source);
            else{
                _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
            }
        }
    };

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_add_epi16(
                    _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                    _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_add_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                          _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_sub_epi16(
                    _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                    _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_sub_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                          _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm512_mullo_epi16(
                    _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)), *scalarA),
                    _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm512_mullo_epi16(_mm512_load_si512(reinterpret_cast<const void*>(a + Is * Base::registerSize)),
                                                                                             _mm512_load_si512(reinterpret_cast<const void*>(b + Is * Base::registerSize)))), ...);
        }
    }
};



} // namespace Stalker::Mathematics::SIMD

