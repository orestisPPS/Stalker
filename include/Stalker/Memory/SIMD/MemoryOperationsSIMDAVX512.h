#pragma once

#include <Stalker/Core/Traits/SIMD/SIMDTraitsAVX512.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDBase.h>

namespace Stalker::Memory::SIMD {

// Double Specialization
template<>
struct MemoryOperationsSIMD<double, SIMDType::AVX512>
    : public MemoryOperationsSIMDBase<double, SIMDType::AVX512, MemoryOperationsSIMD<double, SIMDType::AVX512>> {

    using Base = MemoryOperationsSIMDBase<double, SIMDType::AVX512, MemoryOperationsSIMD<double, SIMDType::AVX512>>;
    friend Base;

private:
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm512_store_pd(destination, source);
            else
                _mm512_stream_pd(destination, source);
        }
    };

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* __restrict source, Base::T_simd* __restrict destination, std::index_sequence<Is...>) {
        ((destination[Is] = _mm512_load_pd(source + Is * Base::registerSize)), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _store(const Base::T_simd* __restrict source, Base::T_data* __restrict destination, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(destination + Is * Base::registerSize, source[Is])), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict source, Base::T_data* __restrict destination, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(destination + Is * Base::registerSize, _mm512_load_pd(source + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict destination, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(destination + Is * Base::registerSize, _mm512_setzero_pd())), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setValue(Base::T_data* __restrict destination, const Base::T_data& value, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(destination + Is * Base::registerSize, _mm512_set1_pd(value))), ...);
    }

    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data* a, const Base::T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result && _mm512_test_epi64_mask(
            _mm512_castpd_si512(_mm512_load_pd(a + Is * Base::registerSize)),
            _mm512_castpd_si512(_mm512_load_pd(b + Is * Base::registerSize))
        ) == 0xFF), ...);
        return result;
    }
};

// Float Specialization
template<>
struct MemoryOperationsSIMD<float, SIMDType::AVX512>
    : public MemoryOperationsSIMDBase<float, SIMDType::AVX512, MemoryOperationsSIMD<float, SIMDType::AVX512>> {

    using Base = MemoryOperationsSIMDBase<float, SIMDType::AVX512, MemoryOperationsSIMD<float, SIMDType::AVX512>>;
    friend Base;

private:
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm512_store_ps(destination, source);
            else
                _mm512_stream_ps(destination, source);
        }
    };

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm512_load_ps(src + Is * Base::registerSize)), ...);
    }
    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _store(const Base::T_simd* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, src[Is])), ...);
    }
    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_load_ps(src + Is * Base::registerSize))), ...);
    }
    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_setzero_ps())), ...);
    }
    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setValue(Base::T_data* dst, const Base::T_data& val, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_set1_ps(val))), ...);
    }
    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data* a, const Base::T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result && _mm512_test_epi32_mask(
            _mm512_castps_si512(_mm512_load_ps(a + Is * Base::registerSize)),
            _mm512_castps_si512(_mm512_load_ps(b + Is * Base::registerSize))
        ) == 0xFFFF), ...);
        return result;
    }
};

// Int Specialization
template<>
struct MemoryOperationsSIMD<int, SIMDType::AVX512>
    : public MemoryOperationsSIMDBase<int, SIMDType::AVX512, MemoryOperationsSIMD<int, SIMDType::AVX512>> {

    using Base = MemoryOperationsSIMDBase<int, SIMDType::AVX512, MemoryOperationsSIMD<int, SIMDType::AVX512>>;
    friend Base;

private:
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm512_store_si512(reinterpret_cast<__m512i*>(destination), source);
            else
                _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
        }
    };

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm512_load_si512(reinterpret_cast<const __m512i*>(src + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _store(const Base::T_simd* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, src[Is])), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_load_si512(reinterpret_cast<const __m512i*>(src + Is * Base::registerSize)))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_setzero_si512())), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setValue(Base::T_data* __restrict dst, const Base::T_data& val, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_set1_epi32(val))), ...);
    }

    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data*  a, const Base::T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm512_test_epi32_mask(
                _mm512_load_si512(reinterpret_cast<const __m512i*>(a + Is * Base::registerSize)),
                _mm512_load_si512(reinterpret_cast<const __m512i*>(b + Is * Base::registerSize))
            ) == 0xFFFF), ...);
        return result;
    }
};

// Unsigned Int Specialization
template<>
struct MemoryOperationsSIMD<unsigned, SIMDType::AVX512>
    : public MemoryOperationsSIMDBase<unsigned, SIMDType::AVX512, MemoryOperationsSIMD<unsigned, SIMDType::AVX512>> {
    using Base = MemoryOperationsSIMDBase<unsigned, SIMDType::AVX512, MemoryOperationsSIMD<unsigned, SIMDType::AVX512>>;
private:
    friend Base;
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm512_store_si512(reinterpret_cast<__m512i*>(destination), source);
            else
                _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
        }
    };

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm512_load_si512(reinterpret_cast<const __m512i*>(src + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _store(const Base::T_simd* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, src[Is])), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_load_si512(reinterpret_cast<const __m512i*>(src + Is * Base::registerSize)))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_setzero_si512())), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setValue(Base::T_data* __restrict dst, const Base::T_data& val, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_set1_epi32(val))), ...);
    }

    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data*  a, const Base::T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm512_test_epi32_mask(
                _mm512_load_si512(reinterpret_cast<const __m512i*>(a + Is * Base::registerSize)),
                _mm512_load_si512(reinterpret_cast<const __m512i*>(b + Is * Base::registerSize))
            ) == 0xFFFF), ...);
        return result;
    }
};

// Short Specialization
template<>
struct MemoryOperationsSIMD<short, SIMDType::AVX512>
    : public MemoryOperationsSIMDBase<short, SIMDType::AVX512, MemoryOperationsSIMD<short, SIMDType::AVX512>> {

    using Base = MemoryOperationsSIMDBase<short, SIMDType::AVX512, MemoryOperationsSIMD<short, SIMDType::AVX512>>;
private:

    friend Base;
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm512_store_si512(reinterpret_cast<__m512i*>(destination), source);
            else
                _mm512_stream_si512(reinterpret_cast<__m512i*>(destination), source);
        }
    };

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm512_load_si512(reinterpret_cast<const __m512i*>(src + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _store(const Base::T_simd* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, src[Is])), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_load_si512(reinterpret_cast<const __m512i*>(src + Is * Base::registerSize)))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_setzero_si512())), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setValue(Base::T_data* __restrict dst, const Base::T_data& val, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm512_set1_epi16(val))), ...);
    }

    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data*  a, const Base::T_data* b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm512_test_epi16_mask(
                _mm512_load_si512(reinterpret_cast<const __m512i*>(a + Is * Base::registerSize)),
                _mm512_load_si512(reinterpret_cast<const __m512i*>(b + Is * Base::registerSize))
            ) == 0xFFFF), ...);
        return result;
    }
};

} // namespace Stalker::Memory