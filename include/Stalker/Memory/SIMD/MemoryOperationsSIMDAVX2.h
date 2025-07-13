#pragma once

#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDAVX2.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDBase.h>

namespace Stalker::Memory::SIMD {
    
template<>
struct MemoryOperationsSIMD<double, SIMDType::AVX2>
    : public MemoryOperationsSIMDBase<double, SIMDType::AVX2, MemoryOperationsSIMD<double, SIMDType::AVX2>> {

    using Base = MemoryOperationsSIMDBase<double, SIMDType::AVX2, MemoryOperationsSIMD<double, SIMDType::AVX2>>;
    friend Base;

private:

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* __restrict source, Base::T_simd* __restrict destination, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_load_pd(source + Is * Base::registerSize)), ...);
    }

    template <SIMDStoreType Policy>
    static constexpr inline void _store(Base::T_data* __restrict destination, const Base::T_simd source) {
        if constexpr (Policy == SIMDStoreType::Cached)
            _mm256_store_pd(destination, source);
        else
            _mm256_stream_pd(destination, source);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict source, Base::T_data* __restrict destination, std::index_sequence<Is...>) {
        ((_store<Policy>(destination + Is * Base::registerSize, _mm256_load_pd(source + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict destination, std::index_sequence<Is...>) {
        ((_store<Policy>(destination + Is * Base::registerSize, _mm256_setzero_pd())), ...);
    }

    template< SIMDStoreType Policy, size_t... Is >
    static constexpr inline void _setValue( Base::T_data* __restrict dst, const Base::T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + Is * Base::registerSize, scalarSIMD[Is] ), ... );
    }

    template <size_t... Is>
    static inline void _broadcast(Base::T_simd* __restrict destination, const Base::T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_set1_pd(value)), ...);
    }

    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data*  a, const Base::T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result && _mm256_testc_pd(_mm256_load_pd(a + Is * Base::registerSize), _mm256_load_pd(b + Is * Base::registerSize))), ...);
        return result;
    };
};

// Float Specialization
template<>
struct MemoryOperationsSIMD<float, SIMDType::AVX2>
    : public MemoryOperationsSIMDBase<float, SIMDType::AVX2, MemoryOperationsSIMD<float, SIMDType::AVX2>> {

    using Base = MemoryOperationsSIMDBase<float, SIMDType::AVX2, MemoryOperationsSIMD<float, SIMDType::AVX2>>;

private:
    friend Base;

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_ps(src + Is * Base::registerSize)), ...);
    }

    template <SIMDStoreType Policy>
    static constexpr inline void _store(Base::T_data* __restrict destination, const Base::T_simd source) {
        if constexpr (Policy == SIMDStoreType::Cached)
            _mm256_store_ps(destination, source);
        else
            _mm256_stream_ps(destination, source);
    }   
    
    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_load_ps(src + Is * Base::registerSize))), ...);
    }
    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_setzero_ps())), ...);
    }
    template< SIMDStoreType Policy, size_t... Is >
    static constexpr inline void _setValue( Base::T_data* __restrict dst, const Base::T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + Is * Base::registerSize, scalarSIMD[Is] ), ... );
    }
    template <size_t... Is>
    static  inline void _broadcast(Base::T_simd* __restrict destination, const Base::T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_set1_ps(value)), ...);
    }
    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data* a, const Base::T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result && _mm256_testc_ps(_mm256_load_ps(a + Is * Base::registerSize), _mm256_load_ps(b + Is * Base::registerSize))), ...);
        return result;
    }
};

// Int Specialization
template<>
struct MemoryOperationsSIMD<int, SIMDType::AVX2>
    : public MemoryOperationsSIMDBase<int, SIMDType::AVX2, MemoryOperationsSIMD<int, SIMDType::AVX2>> {

    using Base = MemoryOperationsSIMDBase<int, SIMDType::AVX2, MemoryOperationsSIMD<int, SIMDType::AVX2>>;

private:
    friend Base;

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy>
    static constexpr inline void _store(Base::T_data* __restrict destination, const Base::T_simd source) {
        if constexpr (Policy == SIMDStoreType::Cached)
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
        else
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize)))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_setzero_si256())), ...);
    }

    template< SIMDStoreType Policy, size_t... Is >
    static constexpr inline void _setValue( Base::T_data* __restrict dst, const Base::T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + Is * Base::registerSize, scalarSIMD[Is] ), ... );
    }
    
    template <size_t... Is>
    static inline void _broadcast(Base::T_simd* __restrict destination, const Base::T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_set1_epi32(value)), ...);
    }

    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data*  a, const Base::T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm256_testc_si256(
                _mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize))
            )), ...);
        return result;
    }

};

template<>
struct MemoryOperationsSIMD<unsigned, SIMDType::AVX2>
    : public MemoryOperationsSIMDBase<unsigned, SIMDType::AVX2, MemoryOperationsSIMD<unsigned, SIMDType::AVX2>> {
    using Base = MemoryOperationsSIMDBase<unsigned, SIMDType::AVX2, MemoryOperationsSIMD<unsigned, SIMDType::AVX2>>;
private:
    friend Base;

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy>
    static constexpr inline void _store(Base::T_data* __restrict destination, const Base::T_simd source) {
        if constexpr (Policy == SIMDStoreType::Cached)
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
        else
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize)))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_setzero_si256())), ...);
    }

    template< SIMDStoreType Policy, size_t... Is >
    static constexpr inline void _setValue( Base::T_data* __restrict dst, const Base::T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + Is * Base::registerSize, scalarSIMD[Is] ), ... );
    }

    template <size_t... Is>
    static inline void _broadcast(Base::T_simd* __restrict destination, const Base::T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_set1_epi32(value)), ...);
    }

    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data*  a, const Base::T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm256_testc_si256(
                _mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize))
            )), ...);
        return result;
    }
};

// Short Specialization
template<>
struct MemoryOperationsSIMD<short, SIMDType::AVX2>
    : public MemoryOperationsSIMDBase<short, SIMDType::AVX2, MemoryOperationsSIMD<short, SIMDType::AVX2>> {

    using Base = MemoryOperationsSIMDBase<short, SIMDType::AVX2, MemoryOperationsSIMD<short, SIMDType::AVX2>>;

private:

    friend Base;

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy>
    static constexpr inline void _store(Base::T_data* __restrict destination, const Base::T_simd source) {
        if constexpr (Policy == SIMDStoreType::Cached)
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
        else
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize)))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_setzero_si256())), ...);
    }

    template< SIMDStoreType Policy, size_t... Is >
    static constexpr inline void _setValue( Base::T_data* __restrict dst, const Base::T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + Is * Base::registerSize, scalarSIMD[Is] ), ... );
    }

    template <size_t... Is>
    static inline void _broadcast(Base::T_simd* __restrict destination, const Base::T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_set1_epi16(value)), ...);
    }
    
    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data*  a, const Base::T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm256_testc_si256(
                _mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize))
            )), ...);
        return result;
    }
};


}