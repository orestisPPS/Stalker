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
    static STALKER_FORCE_INLINE T_simd _load(const T_data* STALKER_RESTRICT source) {
        if constexpr (IsAligned)
            return _mm512_load_pd(source);
        else
            return _mm512_loadu_pd(source);
    }

    template <T_SIMDStore Policy>
    static STALKER_FORCE_INLINE void _store(T_data* STALKER_RESTRICT destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm512_store_pd(destination, source);
        else
            _mm512_stream_pd(destination, source);
    }

    static STALKER_FORCE_INLINE void _setZeroRegister(T_simd* STALKER_RESTRICT destination) {
        *destination = _mm512_setzero_pd();
    }

    template <size_t... Is>
    static STALKER_FORCE_INLINE void _broadcast(T_simd* STALKER_RESTRICT destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm512_set1_pd(value)), ...);
    }

    template <bool IsAligned, size_t... Is>
    static STALKER_FORCE_INLINE bool _areEqual(const T_data* a, const T_data* b, std::index_sequence<Is...>) {
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
    static STALKER_FORCE_INLINE T_simd _load(const T_data* STALKER_RESTRICT source) {
        if constexpr (IsAligned)
            return _mm512_load_ps(source);
        else
            return _mm512_loadu_ps(source);
    }

    template <T_SIMDStore Policy>
    static STALKER_FORCE_INLINE void _store(T_data* STALKER_RESTRICT destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm512_store_ps(destination, source);
        else
            _mm512_stream_ps(destination, source);
    }

    static STALKER_FORCE_INLINE void _setZeroRegister(T_simd* STALKER_RESTRICT destination) {
        *destination = _mm512_setzero_ps();
    }

    template <size_t... Is>
    static  STALKER_FORCE_INLINE void _broadcast(T_simd* STALKER_RESTRICT destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm512_set1_ps(value)), ...);
    }

    template <bool IsAligned, size_t... Is>
    static STALKER_FORCE_INLINE bool _areEqual(const T_data* a, const T_data* b, std::index_sequence<Is...>) {
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
    static STALKER_FORCE_INLINE T_simd _load(const T_data* STALKER_RESTRICT source) {
        if constexpr (IsAligned)
            return _mm512_load_si512(reinterpret_cast<const __m512i*>(source));
        else
            return _mm512_loadu_si512(reinterpret_cast<const __m512i*>(source));
    }

    template <T_SIMDStore Policy>
    static STALKER_FORCE_INLINE void _store(T_data* STALKER_RESTRICT destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm512_store_si512(reinterpret_cast<__m512i*>(destination), source);
        else
            _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
    }

    static STALKER_FORCE_INLINE void _setZeroRegister(T_simd* STALKER_RESTRICT destination) {
        *destination = _mm512_setzero_si512();
    }

    template <size_t... Is>
    static STALKER_FORCE_INLINE void _broadcast(T_simd* STALKER_RESTRICT destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm512_set1_epi32(value)), ...);
    }

    template <bool IsAligned, size_t... Is>
    static STALKER_FORCE_INLINE bool _areEqual(const T_data*  a, const T_data* b, std::index_sequence<Is...>) {
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
    static STALKER_FORCE_INLINE T_simd _load(const T_data* STALKER_RESTRICT source) {
        if constexpr (IsAligned)
            return _mm512_load_si512(reinterpret_cast<const __m512i*>(source));
        else
            return _mm512_loadu_si512(reinterpret_cast<const __m512i*>(source));
    }

    template <T_SIMDStore Policy>
    static STALKER_FORCE_INLINE void _store(T_data* STALKER_RESTRICT destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm512_store_si512(reinterpret_cast<__m512i*>(destination), source);
        else
            _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
    }

    static STALKER_FORCE_INLINE void _setZeroRegister(T_simd* STALKER_RESTRICT destination) {
        *destination = _mm512_setzero_si512();
    }

    template <size_t... Is>
    static STALKER_FORCE_INLINE void _broadcast(T_simd* STALKER_RESTRICT destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm512_set1_epi32(value)), ...);
    }


    template <bool IsAligned, size_t... Is>
    static STALKER_FORCE_INLINE bool _areEqual(const T_data*  a, const T_data* b, std::index_sequence<Is...>) {
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
    static STALKER_FORCE_INLINE T_simd _load(const T_data* STALKER_RESTRICT source) {
        if constexpr (IsAligned)
            return _mm512_load_si512(reinterpret_cast<const __m512i*>(source));
        else
            return _mm512_loadu_si512(reinterpret_cast<const __m512i*>(source));
    }

    template <T_SIMDStore Policy>
    static STALKER_FORCE_INLINE void _store(T_data* STALKER_RESTRICT destination, const T_simd& source) {
        if constexpr (Policy == T_SIMDStore::Cached)
            _mm512_store_si512(reinterpret_cast<__m512i*>(destination), source);
        else
            _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
    }

    static STALKER_FORCE_INLINE void _setZeroRegister(T_simd* STALKER_RESTRICT destination) {
        *destination = _mm512_setzero_si512();
    }

    template <size_t... Is>
    static STALKER_FORCE_INLINE void _broadcast(T_simd* STALKER_RESTRICT destination, const T_data& value, std::index_sequence<Is...>) {
        ((destination[Is] = _mm512_set1_epi16(value)), ...);
    }

    template <bool IsAligned, size_t... Is>
    static STALKER_FORCE_INLINE bool _areEqual(const T_data*  a, const T_data* b, std::index_sequence<Is...>) {
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