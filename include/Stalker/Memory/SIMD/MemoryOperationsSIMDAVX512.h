/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDAVX512.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDBase.h>

namespace Stalker::Memory {

// Double Specialization
template<>
struct MemoryOperationsSIMD<double, T_SIMD::AVX512>
    : public MemoryOperationsSIMDBase<double, T_SIMD::AVX512, MemoryOperationsSIMD<double, T_SIMD::AVX512>> {

    using Base = MemoryOperationsSIMDBase<double, T_SIMD::AVX512, MemoryOperationsSIMD<double, T_SIMD::AVX512>>;
    using Traits = TypeTraitsSIMD<double, T_SIMD::AVX512>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;

private:

    friend Base;
    
    template <bool IsAligned = false>
    static inline T_simd _load(const T_data* __restrict source) {
        if constexpr (IsAligned)
            return _mm512_load_pd(source);
        else
            return _mm512_loadu_pd(source);
    }

    template <T_SIMDStore Policy>
    static inline void _store(T_data* __restrict destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm512_store_pd(destination, source);
        else
            _mm512_stream_pd(destination, source);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _copy(const T_data* __restrict source, T_data* __restrict destination, std::index_sequence<Is...>) {
        ((storeOffset<Is, Policy>(destination, loadOffset<Is, IsAligned>(source))), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _setZero(T_data* __restrict destination, std::index_sequence<Is...>) {
        ((storeOffset<Is, Policy>(destination, _mm512_setzero_pd())), ...);
    }

    static inline void _setZeroRegister(T_simd* __restrict destination) {
        *destination = _mm512_setzero_pd();
    }

    template<bool IsAligned, T_SIMDStore Policy, size_t... Is >
    static inline void _setValue( T_data* __restrict dst, const T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( storeOffset<Is, Policy>( dst, *scalarSIMD ), ... );
    }

    template <size_t... Is>
    static inline void _broadcast(T_simd* __restrict destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm512_set1_pd(value)), ...);
    }

    template <bool IsAligned, size_t... Is>
    static inline bool _areEqual(const T_data* a, const T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result && _mm512_test_epi64_mask(
            _mm512_castpd_si512(_mm512_load_pd(a + _registerOffset<Is>())),
            _mm512_castpd_si512(_mm512_load_pd(b + _registerOffset<Is>()))
        ) == 0xFF), ...);
        return result;
    }
};

// Float Specialization
template<>
struct MemoryOperationsSIMD<float, T_SIMD::AVX512>
    : public MemoryOperationsSIMDBase<float, T_SIMD::AVX512, MemoryOperationsSIMD<float, T_SIMD::AVX512>> {

    using Base = MemoryOperationsSIMDBase<float, T_SIMD::AVX512, MemoryOperationsSIMD<float, T_SIMD::AVX512>>;
    using Traits = TypeTraitsSIMD<float, T_SIMD::AVX512>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;

private:
    friend Base;

    template <bool IsAligned = false>
    static inline T_simd _load(const T_data* __restrict source) {
        if constexpr (IsAligned)
            return _mm512_load_ps(source);
        else
            return _mm512_loadu_ps(source);
    }

    template <T_SIMDStore Policy>
    static inline void _store(T_data* __restrict destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm512_store_ps(destination, source);
        else
            _mm512_stream_ps(destination, source);
    }
    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _copy(const T_data* src, T_data* dst, std::index_sequence<Is...>) {
        ((storeOffset<Is, Policy>(dst, loadOffset<Is, IsAligned>(src))), ...);
    }
    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _setZero(T_data* dst, std::index_sequence<Is...>) {
        ((storeOffset<Is, Policy>(dst, _mm512_setzero_ps())), ...);
    }
    static inline void _setZeroRegister(T_simd* __restrict destination) {
        *destination = _mm512_setzero_ps();
    }
    template<bool IsAligned, T_SIMDStore Policy, size_t... Is >
    static inline void _setValue( T_data* __restrict dst, const T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( storeOffset<Is, Policy>( dst, *scalarSIMD ), ... );
    }
    template <size_t... Is>
    static  inline void _broadcast(T_simd* __restrict destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm512_set1_ps(value)), ...);
    }
    template <bool IsAligned, size_t... Is>
    static inline bool _areEqual(const T_data* a, const T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result && _mm512_test_epi32_mask(
            _mm512_castps_si512(_mm512_load_ps(a + _registerOffset<Is>())),
            _mm512_castps_si512(_mm512_load_ps(b + _registerOffset<Is>()))
        ) == 0xFFFF), ...);
        return result;
    }
};

// Int Specialization
template<>
struct MemoryOperationsSIMD<int, T_SIMD::AVX512>
    : public MemoryOperationsSIMDBase<int, T_SIMD::AVX512, MemoryOperationsSIMD<int, T_SIMD::AVX512>> {

    using Base = MemoryOperationsSIMDBase<int, T_SIMD::AVX512, MemoryOperationsSIMD<int, T_SIMD::AVX512>>;
    using Traits = TypeTraitsSIMD<int, T_SIMD::AVX512>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;

private:
    friend Base;

    template <bool IsAligned = false>
    static inline T_simd _load(const T_data* __restrict source) {
        if constexpr (IsAligned)
            return _mm512_load_si512(reinterpret_cast<const __m512i*>(source));
        else
            return _mm512_loadu_si512(reinterpret_cast<const __m512i*>(source));
    }

    template <T_SIMDStore Policy>
    static inline void _store(T_data* __restrict destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm512_store_si512(reinterpret_cast<__m512i*>(destination), source);
        else
            _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _copy(const T_data* __restrict src, T_data* __restrict dst, std::index_sequence<Is...>) {
        ((storeOffset<Is, Policy>(dst, loadOffset<Is, IsAligned>(src))), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _setZero(T_data* __restrict dst, std::index_sequence<Is...>) {
        ((storeOffset<Is, Policy>(dst, _mm512_setzero_si512())), ...);
    }

    static inline void _setZeroRegister(T_simd* __restrict destination) {
        *destination = _mm512_setzero_si512();
    }

    template<bool IsAligned, T_SIMDStore Policy, size_t... Is >
    static inline void _setValue( T_data* __restrict dst, const T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( storeOffset<Is, Policy>( dst, *scalarSIMD ), ... );
    }
    template <size_t... Is>
    static inline void _broadcast(T_simd* __restrict destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm512_set1_epi32(value)), ...);
    }

    template <bool IsAligned, size_t... Is>
    static inline bool _areEqual(const T_data*  a, const T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm512_test_epi32_mask(
                _mm512_load_si512(reinterpret_cast<const __m512i*>(a + _registerOffset<Is>())),
                _mm512_load_si512(reinterpret_cast<const __m512i*>(b + _registerOffset<Is>()))
            ) == 0xFFFF), ...);
        return result;
    }
};

// Unsigned Int Specialization
template<>
struct MemoryOperationsSIMD<unsigned, T_SIMD::AVX512>
    : public MemoryOperationsSIMDBase<unsigned, T_SIMD::AVX512, MemoryOperationsSIMD<unsigned, T_SIMD::AVX512>> {
    
    using Base = MemoryOperationsSIMDBase<unsigned, T_SIMD::AVX512, MemoryOperationsSIMD<unsigned, T_SIMD::AVX512>>;
    using Traits = TypeTraitsSIMD<unsigned, T_SIMD::AVX512>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;

private:

    friend Base;

    template <bool IsAligned = false>
    static inline T_simd _load(const T_data* __restrict source) {
        if constexpr (IsAligned)
            return _mm512_load_si512(reinterpret_cast<const __m512i*>(source));
        else
            return _mm512_loadu_si512(reinterpret_cast<const __m512i*>(source));
    }

    template <T_SIMDStore Policy>
    static inline void _store(T_data* __restrict destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm512_store_si512(reinterpret_cast<__m512i*>(destination), source);
        else
            _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _copy(const T_data* __restrict src, T_data* __restrict dst, std::index_sequence<Is...>) {
        ((storeOffset<Is, Policy>(dst, loadOffset<Is, IsAligned>(src))), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _setZero(T_data* __restrict dst, std::index_sequence<Is...>) {
        ((storeOffset<Is, Policy>(dst, _mm512_setzero_si512())), ...);
    }

    static inline void _setZeroRegister(T_simd* __restrict destination) {
        *destination = _mm512_setzero_si512();
    }

    template<bool IsAligned, T_SIMDStore Policy, size_t... Is >
    static inline void _setValue( T_data* __restrict dst, const T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( storeOffset<Is, Policy>( dst, *scalarSIMD ), ... );
    }

    template <size_t... Is>
    static inline void _broadcast(T_simd* __restrict destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm512_set1_epi32(value)), ...);
    }


    template <bool IsAligned, size_t... Is>
    static inline bool _areEqual(const T_data*  a, const T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm512_test_epi32_mask(
                _mm512_load_si512(reinterpret_cast<const __m512i*>(a + _registerOffset<Is>())),
                _mm512_load_si512(reinterpret_cast<const __m512i*>(b + _registerOffset<Is>()))
            ) == 0xFFFF), ...);
        return result;
    }
};

// Short Specialization
template<>
struct MemoryOperationsSIMD<short, T_SIMD::AVX512>
    : public MemoryOperationsSIMDBase<short, T_SIMD::AVX512, MemoryOperationsSIMD<short, T_SIMD::AVX512>> {

    using Base = MemoryOperationsSIMDBase<short, T_SIMD::AVX512, MemoryOperationsSIMD<short, T_SIMD::AVX512>>;
private:

    friend Base;

    template <bool IsAligned = false>
    static inline T_simd _load(const T_data* __restrict source) {
        if constexpr (IsAligned)
            return _mm512_load_si512(reinterpret_cast<const __m512i*>(source));
        else
            return _mm512_loadu_si512(reinterpret_cast<const __m512i*>(source));
    }

    template <T_SIMDStore Policy>
    static inline void _store(T_data* __restrict destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm512_store_si512(reinterpret_cast<__m512i*>(destination), source);
        else
            _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _copy(const T_data* __restrict src, T_data* __restrict dst, std::index_sequence<Is...>) {
        ((storeOffset<Is, Policy>(dst, loadOffset<Is, IsAligned>(src))), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _setZero(T_data* __restrict dst, std::index_sequence<Is...>) {
        ((storeOffset<Is, Policy>(dst, _mm512_setzero_si512())), ...);
    }

    static inline void _setZeroRegister(T_simd* __restrict destination) {
        *destination = _mm512_setzero_si512();
    }

    template<bool IsAligned, T_SIMDStore Policy, size_t... Is >
    static inline void _setValue( T_data* __restrict dst, const T_simd* __restrict scalarSIMD, std::index_sequence<Is...> ){
        ( _store<Policy>( dst + _registerOffset<Is>(), *scalarSIMD ), ... );
    }
    template <size_t... Is>
    static inline void _broadcast(T_simd* __restrict destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm512_set1_epi16(value)), ...);
    }

    template <bool IsAligned, size_t... Is>
    static inline bool _areEqual(const T_data*  a, const T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm512_test_epi16_mask(
                _mm512_load_si512(reinterpret_cast<const __m512i*>(a + _registerOffset<Is>())),
                _mm512_load_si512(reinterpret_cast<const __m512i*>(b + _registerOffset<Is>()))
            ) == 0xFFFF), ...);
        return result;
    }
};

} // namespace Stalker::Memory