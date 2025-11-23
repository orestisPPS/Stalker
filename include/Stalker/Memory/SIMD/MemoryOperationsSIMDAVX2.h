#pragma once

#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDAVX2.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDBase.h>

namespace Stalker::Memory {
    
template<>
struct MemoryOperationsSIMD<double, T_SIMD::AVX2>
    : public MemoryOperationsSIMDBase<double, T_SIMD::AVX2, MemoryOperationsSIMD<double, T_SIMD::AVX2>> {

    using Base = MemoryOperationsSIMDBase<double, T_SIMD::AVX2, MemoryOperationsSIMD<double, T_SIMD::AVX2>>;
    using Traits = TypeTraitsSIMD<double, T_SIMD::AVX2>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;

private:
    
    friend Base;

    template <bool IsAligned = false>
    static inline T_simd _load(const T_data* __restrict source) {
        if constexpr (IsAligned)
            return _mm256_load_pd(source);
        else
            return _mm256_loadu_pd(source);
    }

    template <T_SIMDStore Policy>
    static inline void _store(T_data* __restrict destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm256_store_pd(destination, source);
        else
            _mm256_stream_pd(destination, source);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _copy(const T_data* __restrict source, T_data* __restrict destination, std::index_sequence<Is...>) {
        ((_store<Policy>(destination + _registerOffset<Is>(), _mm256_load_pd(source + _registerOffset<Is>()))), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _setZero(T_data* __restrict destination, std::index_sequence<Is...>) {
        ((_store<Policy>(destination + _registerOffset<Is>(), _mm256_setzero_pd())), ...);
    }

    static inline void _setZeroRegister(T_simd* __restrict destination) {
        *destination = _mm256_setzero_pd();
    }

    template< T_SIMDStore Policy, size_t... Is >
    static inline void _setValue( T_data* __restrict dst, const T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + _registerOffset<Is>(), *scalarSIMD ), ... );
    }

    template <size_t... Is>
    static inline void _broadcast(T_simd* __restrict destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_set1_pd(value)), ...);
    }

    template <size_t... Is>
    static inline bool _areEqual(const T_data*  a, const T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result && _mm256_testc_pd(_mm256_load_pd(a + _registerOffset<Is>()), _mm256_load_pd(b + _registerOffset<Is>()))), ...);
        return result;
    }
};

// Float Specialization
template<>
struct MemoryOperationsSIMD<float, T_SIMD::AVX2>
    : public MemoryOperationsSIMDBase<float, T_SIMD::AVX2, MemoryOperationsSIMD<float, T_SIMD::AVX2>> {

    using Base = MemoryOperationsSIMDBase<float, T_SIMD::AVX2, MemoryOperationsSIMD<float, T_SIMD::AVX2>>;
    using Traits = TypeTraitsSIMD<float, T_SIMD::AVX2>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;

private:

    friend Base;

    template <bool IsAligned = false>
    static inline T_simd _load(const T_data* __restrict source) {
        if constexpr (IsAligned)
            return _mm256_load_ps(source);
        else
            return _mm256_loadu_ps(source);
    }

    template <T_SIMDStore Policy>
    static inline void _store(T_data* __restrict destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm256_store_ps(destination, source);
        else
            _mm256_stream_ps(destination, source);
    }   
    
    template <T_SIMDStore Policy, size_t... Is>
    static inline void _copy(const T_data* src, T_data* dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + _registerOffset<Is>(), _mm256_load_ps(src + _registerOffset<Is>()))), ...);
    }
    template <T_SIMDStore Policy, size_t... Is>
    static inline void _setZero(T_data* dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + _registerOffset<Is>(), _mm256_setzero_ps())), ...);
    }
    static inline void _setZeroRegister(T_simd* __restrict destination) {
        *destination = _mm256_setzero_ps();
    }
    template< T_SIMDStore Policy, size_t... Is >
    static inline void _setValue( T_data* __restrict dst, const T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + _registerOffset<Is>(), *scalarSIMD ), ... );
    }
    template <size_t... Is>
    static  inline void _broadcast(T_simd* __restrict destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_set1_ps(value)), ...);
    }
    template <size_t... Is>
    static inline bool _areEqual(const T_data* a, const T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result && _mm256_testc_ps(_mm256_load_ps(a + _registerOffset<Is>()), _mm256_load_ps(b + _registerOffset<Is>()))), ...);
        return result;
    }
};

// Int Specialization
template<>
struct MemoryOperationsSIMD<int, T_SIMD::AVX2>
    : public MemoryOperationsSIMDBase<int, T_SIMD::AVX2, MemoryOperationsSIMD<int, T_SIMD::AVX2>> {

    using Base = MemoryOperationsSIMDBase<int, T_SIMD::AVX2, MemoryOperationsSIMD<int, T_SIMD::AVX2>>;
    using Traits = TypeTraitsSIMD<int, T_SIMD::AVX2>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;

private:

    friend Base;

    template <bool IsAligned = false>
    static inline T_simd _load(const T_data* __restrict source) {
        if constexpr (IsAligned)
            return _mm256_load_si256(reinterpret_cast<const __m256i*>(source));
        else
            return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(source));
    }

    template <T_SIMDStore Policy>
    static inline void _store(T_data* __restrict destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
        else
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _copy(const T_data* __restrict src, T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + _registerOffset<Is>(), _mm256_load_si256(reinterpret_cast<const __m256i*>(src + _registerOffset<Is>())))), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _setZero(T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + _registerOffset<Is>(), _mm256_setzero_si256())), ...);
    }

    static inline void _setZeroRegister(T_simd* __restrict destination) {
        *destination = _mm256_setzero_si256();
    }

    template< T_SIMDStore Policy, size_t... Is >
    static inline void _setValue( T_data* __restrict dst, const T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + _registerOffset<Is>(), *scalarSIMD ), ... );
    }
    
    template <size_t... Is>
    static inline void _broadcast(T_simd* __restrict destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_set1_epi32(value)), ...);
    }

    template <size_t... Is>
    static inline bool _areEqual(const T_data*  a, const T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm256_testc_si256(
                _mm256_load_si256(reinterpret_cast<const __m256i*>(a + _registerOffset<Is>())),
                _mm256_load_si256(reinterpret_cast<const __m256i*>(b + _registerOffset<Is>()))
            )), ...);
        return result;
    }

};

template<>
struct MemoryOperationsSIMD<unsigned, T_SIMD::AVX2>
    : public MemoryOperationsSIMDBase<unsigned, T_SIMD::AVX2, MemoryOperationsSIMD<unsigned, T_SIMD::AVX2>> {
    
    using Base = MemoryOperationsSIMDBase<unsigned, T_SIMD::AVX2, MemoryOperationsSIMD<unsigned, T_SIMD::AVX2>>;
    using Traits = TypeTraitsSIMD<unsigned, T_SIMD::AVX2>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;

private:

    friend Base;

    template <bool IsAligned = false>
    static inline T_simd _load(const T_data* __restrict source) {
        if constexpr (IsAligned)
            return _mm256_load_si256(reinterpret_cast<const __m256i*>(source));
        else
            return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(source));
    }

    template <T_SIMDStore Policy>
    static inline void _store(T_data* __restrict destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
        else
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _copy(const T_data* __restrict src, T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + _registerOffset<Is>(), _mm256_load_si256(reinterpret_cast<const __m256i*>(src + _registerOffset<Is>())))), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _setZero(T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + _registerOffset<Is>(), _mm256_setzero_si256())), ...);
    }

    static inline void _setZeroRegister(T_simd* __restrict destination) {
        *destination = _mm256_setzero_si256();
    }

    template< T_SIMDStore Policy, size_t... Is >
    static inline void _setValue( T_data* __restrict dst, const T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + _registerOffset<Is>(), *scalarSIMD ), ... );
    }

    template <size_t... Is>
    static inline void _broadcast(T_simd* __restrict destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_set1_epi32(value)), ...);
    }

    template <size_t... Is>
    static inline bool _areEqual(const T_data*  a, const T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm256_testc_si256(
                _mm256_load_si256(reinterpret_cast<const __m256i*>(a + _registerOffset<Is>())),
                _mm256_load_si256(reinterpret_cast<const __m256i*>(b + _registerOffset<Is>()))
            )), ...);
        return result;
    }
};

// Short Specialization
template<>
struct MemoryOperationsSIMD<short, T_SIMD::AVX2>
    : public MemoryOperationsSIMDBase<short, T_SIMD::AVX2, MemoryOperationsSIMD<short, T_SIMD::AVX2>> {

    using Base = MemoryOperationsSIMDBase<short, T_SIMD::AVX2, MemoryOperationsSIMD<short, T_SIMD::AVX2>>;
    using Traits = TypeTraitsSIMD<short, T_SIMD::AVX2>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;

private:

    friend Base;

    template <bool IsAligned = false>
    static inline T_simd _load(const T_data* __restrict source) {
        if constexpr (IsAligned)
            return _mm256_load_si256(reinterpret_cast<const __m256i*>(source));
        else
            return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(source));
    }

    template <T_SIMDStore Policy>
    static inline void _store(T_data* __restrict destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
        else
            _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _copy(const T_data* __restrict src, T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + _registerOffset<Is>(), _mm256_load_si256(reinterpret_cast<const __m256i*>(src + _registerOffset<Is>())))), ...);
    }

    template <T_SIMDStore Policy, size_t... Is>
    static inline void _setZero(T_data* __restrict dst, std::index_sequence<Is...>) {
        ((_store<Policy>(dst + _registerOffset<Is>(), _mm256_setzero_si256())), ...);
    }

    static inline void _setZeroRegister(T_simd* __restrict destination) {
        *destination = _mm256_setzero_si256();
    }

    template< T_SIMDStore Policy, size_t... Is >
    static inline void _setValue( T_data* __restrict dst, const T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + _registerOffset<Is>(), *scalarSIMD ), ... );
    }

    template <size_t... Is>
    static inline void _broadcast(T_simd* __restrict destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_set1_epi16(value)), ...);
    }
    
    template <size_t... Is>
    static inline bool _areEqual(const T_data*  a, const T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm256_testc_si256(
                _mm256_load_si256(reinterpret_cast<const __m256i*>(a + _registerOffset<Is>())),
                _mm256_load_si256(reinterpret_cast<const __m256i*>(b + _registerOffset<Is>()))
            )), ...);
        return result;
    }
};


}