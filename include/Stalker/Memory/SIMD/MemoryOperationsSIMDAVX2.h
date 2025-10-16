#pragma once

#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDAVX2.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDBase.h>

namespace Stalker::Memory::SIMD {
    
template<>
struct MemoryOperationsSIMD<double, T_SIMD::AVX2>
    : public MemoryOperationsSIMDBase<double, T_SIMD::AVX2, MemoryOperationsSIMD<double, T_SIMD::AVX2>> {

    using Base = MemoryOperationsSIMDBase<double, T_SIMD::AVX2, MemoryOperationsSIMD<double, T_SIMD::AVX2>>;
    friend Base;

private:

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* __restrict source, Base::T_simd* __restrict destination, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_load_pd(source + Is * Base::registerSize)), ...);
    }

    template <T_SIMDStore Policy>
    static constexpr inline void _store(Base::T_data* __restrict destination, const Base::T_simd source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm256_store_pd(destination, source);
        else
            _mm256_stream_pd(destination, source);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict source, Base::T_data* __restrict destination, std::index_sequence<Is...>) {
        ((_store<Policy>(destination + Is * Base::registerSize, _mm256_load_pd(source + Is * Base::registerSize))), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict destination, std::index_sequence<Is...>) {
        ((_store<Policy>(destination + Is * Base::registerSize, _mm256_setzero_pd())), ...);
    }

    static inline void _setZeroRegister(Base::T_simd* __restrict destination) {
        *destination = _mm256_setzero_pd();
    }

    template< T_SIMDStore Policy, size_t... Is >
    static constexpr inline void _setValue( Base::T_data* __restrict dst, const Base::T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + Is * Base::registerSize, *scalarSIMD ), ... );
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
struct MemoryOperationsSIMD<float, T_SIMD::AVX2>
    : public MemoryOperationsSIMDBase<float, T_SIMD::AVX2, MemoryOperationsSIMD<float, T_SIMD::AVX2>> {

    using Base = MemoryOperationsSIMDBase<float, T_SIMD::AVX2, MemoryOperationsSIMD<float, T_SIMD::AVX2>>;

private:
    friend Base;

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_ps(src + Is * Base::registerSize)), ...);
    }

    template <T_SIMDStore Policy>
    static constexpr inline void _store(Base::T_data* __restrict destination, const Base::T_simd source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm256_store_ps(destination, source);
        else
            _mm256_stream_ps(destination, source);
    }   
    
    template <T_SIMDStore Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_load_ps(src + Is * Base::registerSize))), ...);
    }
    template <T_SIMDStore Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_setzero_ps())), ...);
    }
    static inline void _setZeroRegister(Base::T_simd* __restrict destination) {
        *destination = _mm256_setzero_ps();
    }
    template< T_SIMDStore Policy, size_t... Is >
    static constexpr inline void _setValue( Base::T_data* __restrict dst, const Base::T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + Is * Base::registerSize, *scalarSIMD ), ... );
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
struct MemoryOperationsSIMD<int, T_SIMD::AVX2>
    : public MemoryOperationsSIMDBase<int, T_SIMD::AVX2, MemoryOperationsSIMD<int, T_SIMD::AVX2>> {

    using Base = MemoryOperationsSIMDBase<int, T_SIMD::AVX2, MemoryOperationsSIMD<int, T_SIMD::AVX2>>;

private:
    friend Base;

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize))), ...);
    }

    template <T_SIMDStore Policy>
    static constexpr inline void _store(Base::T_data* __restrict destination, const Base::T_simd source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
        else
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize)))), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_setzero_si256())), ...);
    }

    static inline void _setZeroRegister(Base::T_simd* __restrict destination) {
        *destination = _mm256_setzero_si256();
    }

    template< T_SIMDStore Policy, size_t... Is >
    static constexpr inline void _setValue( Base::T_data* __restrict dst, const Base::T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + Is * Base::registerSize, *scalarSIMD ), ... );
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
struct MemoryOperationsSIMD<unsigned, T_SIMD::AVX2>
    : public MemoryOperationsSIMDBase<unsigned, T_SIMD::AVX2, MemoryOperationsSIMD<unsigned, T_SIMD::AVX2>> {
    using Base = MemoryOperationsSIMDBase<unsigned, T_SIMD::AVX2, MemoryOperationsSIMD<unsigned, T_SIMD::AVX2>>;
private:
    friend Base;

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize))), ...);
    }

    template <T_SIMDStore Policy>
    static constexpr inline void _store(Base::T_data* __restrict destination, const Base::T_simd source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
        else
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize)))), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_setzero_si256())), ...);
    }

    static inline void _setZeroRegister(Base::T_simd* __restrict destination) {
        *destination = _mm256_setzero_si256();
    }

    template< T_SIMDStore Policy, size_t... Is >
    static constexpr inline void _setValue( Base::T_data* __restrict dst, const Base::T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + Is * Base::registerSize, *scalarSIMD ), ... );
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
struct MemoryOperationsSIMD<short, T_SIMD::AVX2>
    : public MemoryOperationsSIMDBase<short, T_SIMD::AVX2, MemoryOperationsSIMD<short, T_SIMD::AVX2>> {

    using Base = MemoryOperationsSIMDBase<short, T_SIMD::AVX2, MemoryOperationsSIMD<short, T_SIMD::AVX2>>;

private:

    friend Base;

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize))), ...);
    }

    template <T_SIMDStore Policy>
    static constexpr inline void _store(Base::T_data* __restrict destination, const Base::T_simd source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
        else
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize)))), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + Is * Base::registerSize, _mm256_setzero_si256())), ...);
    }

    static inline void _setZeroRegister(Base::T_simd* __restrict destination) {
        *destination = _mm256_setzero_si256();
    }

    template< T_SIMDStore Policy, size_t... Is >
    static constexpr inline void _setValue( Base::T_data* __restrict dst, const Base::T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + Is * Base::registerSize, *scalarSIMD ), ... );
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