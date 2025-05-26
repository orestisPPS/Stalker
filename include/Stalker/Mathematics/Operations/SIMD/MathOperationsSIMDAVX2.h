#pragma once

#include <Stalker/Core/Traits/SIMD/SIMDTraitsAVX2.h>
#include <Stalker/Mathematics/Operations/MathOperationsSIMD.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX2.h>

namespace Stalker::Mathematics::SIMD {

using namespace Stalker::Memory::SIMD;
using namespace Stalker::Core;

template<>
struct SIMDMathOperations<double, SIMDType::AVX2>
    : public SIMDMathOperationsBase<double, SIMDType::AVX2, SIMDMathOperations<double, SIMDType::AVX2>> {

    using Base = SIMDMathOperationsBase<double, SIMDType::AVX2, SIMDMathOperations<double, SIMDType::AVX2>>;
    friend Base;
private:

    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm256_store_pd(destination, source);
            else{
                _mm256_stream_pd(destination, source);
            }
        }
    };

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_add_pd(
                    _mm256_mul_pd(_mm256_load_pd(a + Is * Base::registerSize), *scalarA),
                    _mm256_mul_pd(_mm256_load_pd(b + Is * Base::registerSize), *scalarB)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_add_pd(_mm256_load_pd(a + Is * Base::registerSize),
                                                                                          _mm256_load_pd(b + Is * Base::registerSize))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_sub_pd(
                    _mm256_mul_pd(_mm256_load_pd(a + Is * Base::registerSize), *scalarA),
                    _mm256_mul_pd(_mm256_load_pd(b + Is * Base::registerSize), *scalarB)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_sub_pd(_mm256_load_pd(a + Is * Base::registerSize),
                                                                                          _mm256_load_pd(b + Is * Base::registerSize))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_mul_pd(
                    _mm256_load_pd(a + Is * Base::registerSize),
                    _mm256_load_pd(b + Is * Base::registerSize)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_mul_pd(_mm256_load_pd(a + Is * Base::registerSize),
                                                                                          _mm256_load_pd(b + Is * Base::registerSize))), ...);
        }
    }
    
};

template<>
struct SIMDMathOperations<float, SIMDType::AVX2>
    : public SIMDMathOperationsBase<float, SIMDType::AVX2, SIMDMathOperations<float, SIMDType::AVX2>> {

    using Base = SIMDMathOperationsBase<float, SIMDType::AVX2, SIMDMathOperations<float, SIMDType::AVX2>>;
    friend Base;
private:
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm256_store_ps(destination, source);
            else{
                _mm256_stream_ps(destination, source);
            }
        }
    };

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_add_ps(
                    _mm256_mul_ps(_mm256_load_ps(a + Is * Base::registerSize), *scalarA),
                    _mm256_mul_ps(_mm256_load_ps(b + Is * Base::registerSize), *scalarB)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_add_ps(_mm256_load_ps(a + Is * Base::registerSize),
                                                                                          _mm256_load_ps(b + Is * Base::registerSize))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_sub_ps(
                    _mm256_mul_ps(_mm256_load_ps(a + Is * Base::registerSize), *scalarA),
                    _mm256_mul_ps(_mm256_load_ps(b + Is * Base::registerSize), *scalarB)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_sub_ps(_mm256_load_ps(a + Is * Base::registerSize),
                                                                                          _mm256_load_ps(b + Is * Base::registerSize))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_mul_ps(
                    _mm256_load_ps(a + Is * Base::registerSize),
                    _mm256_load_ps(b + Is * Base::registerSize)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_mul_ps(_mm256_load_ps(a + Is * Base::registerSize),
                                                                                          _mm256_load_ps(b + Is * Base::registerSize))), ...);
        }
    }
};

template<>
struct SIMDMathOperations<int, SIMDType::AVX2>
    : public SIMDMathOperationsBase<int, SIMDType::AVX2, SIMDMathOperations<int, SIMDType::AVX2>> {

    using Base = SIMDMathOperationsBase<int, SIMDType::AVX2, SIMDMathOperations<int, SIMDType::AVX2>>;
    friend Base;
private:
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
            else{
                _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
            }
        }
    };

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_add_epi32(
                    _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)), *scalarA),
                    _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_add_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                                                                                          _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_sub_epi32(
                    _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)), *scalarA),
                    _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_sub_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                                                                                          _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_mullo_epi32(
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize))
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize,
                                          _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                                                             _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)))), ...);
        }
    }
};

template<>
struct SIMDMathOperations<unsigned int, SIMDType::AVX2>
    : public SIMDMathOperationsBase<unsigned int, SIMDType::AVX2, SIMDMathOperations<unsigned int, SIMDType::AVX2>> {

    using Base = SIMDMathOperationsBase<unsigned int, SIMDType::AVX2, SIMDMathOperations<unsigned int, SIMDType::AVX2>>;
    friend Base;
private:
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
            else{
                _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
            }
        }
    };

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_add_epi32(
                    _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)), *scalarA),
                    _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_add_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                                                                                          _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_sub_epi32(
                    _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)), *scalarA),
                    _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_sub_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                                                                                             _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_mullo_epi32(
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize))
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_mullo_epi32(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                                                                                             _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)))), ...);
        }
    }
};

template<>
struct SIMDMathOperations<short, SIMDType::AVX2>
    : public SIMDMathOperationsBase<short, SIMDType::AVX2, SIMDMathOperations<short, SIMDType::AVX2>> {

    using Base = SIMDMathOperationsBase<short, SIMDType::AVX2, SIMDMathOperations<short, SIMDType::AVX2>>;
    friend Base;
private:
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
            else{
                _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
            }
        }
    };

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _add(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_add_epi16(
                    _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)), *scalarA),
                    _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_add_epi16(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                                                                                          _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_sub_epi16(
                    _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)), *scalarA),
                    _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)), *scalarB)
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_sub_epi16(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                                                                                          _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)))), ...);
        }
    }

    template<SIMDStoreType Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const Base::T_data* a, const Base::T_data* b, Base::T_data* result, std::index_sequence<Is...>, const Base::T_simd* scalarA = nullptr, const Base::T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(
                result + Is * Base::registerSize,
                _mm256_mullo_epi16(
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize))
                )
            ), ...);
        }
        else {
            // Expand the operation for each index in Is...
            (StoreFunction<Policy>::store(result + Is * Base::registerSize, _mm256_mullo_epi16(_mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                                                                                               _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize)))), ...);
        }
    }
};



} // namespace Stalker::Mathematics::SIMD

