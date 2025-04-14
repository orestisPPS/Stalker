//
// Created by hal9000 on 7/8/24.
//

#ifndef STALKER_MemoryTraits_H
#define STALKER_MemoryTraits_H

#include <utility>
#include "../../../GlobalDefinitions.h"
#include "SIMDTypeTraits.h"

enum class SIMDStoreType {
    Cached,
    Streamed
};

// Forward declarations for specialized SIMDMemoryOperations:
template<typename T, SIMDType Type> struct SIMDMemoryOperations; 
template<> struct SIMDMemoryOperations<float, SIMDType::AVX2>;
template<> struct SIMDMemoryOperations<int, SIMDType::AVX2>;
template<> struct SIMDMemoryOperations<short, SIMDType::AVX2>;
template<> struct SIMDMemoryOperations<unsigned, SIMDType::AVX2>;
template<> struct SIMDMemoryOperations<double, SIMDType::AVX2>;

template<typename T, SIMDType Type, typename Child>
struct SIMDMemoryOperationsBase {

    using T_simd = typename SIMDTypeTraits<T, Type>::typeSIMD;
    using T_data = typename SIMDTypeTraits<T, Type>::typeData;

public:
    static constexpr unsigned blockSize = SIMDTypeTraits<T, Type>::BlockSize;
    static constexpr unsigned registerSize = SIMDTypeTraits<T, Type>::RegisterSize;

    template<unsigned UnrollFactor = UnrollFactorSIMD>
    constexpr inline static void load(const T_data* source, T_simd* destination, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::_load(source + i, destination + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            destination[i] = source[i];
    }

    template<unsigned Size>
    constexpr inline void load(const T_data* source, T_simd* destination) {
        constexpr auto limit = Size - (Size % SIMDTypeTraits<T, Type>::BlockSize);
        Child::_load(source, destination, std::make_index_sequence<limit>{});
        for (size_t i = limit; i < Size; i++)
            destination[i] = source[i];
    }

    template<unsigned UnrollFactor = UnrollFactorSIMD, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline static void store(const T_simd* source, T_data* destination, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::_store<Policy>(source + i, destination + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            destination[i] = source[i];
    }

    template<unsigned Size, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline void store(const T_simd* source, T_data* destination) {
        constexpr auto limit = Size - (Size % SIMDTypeTraits<T, Type>::BlockSize);
        Child::_store<Policy>(source, destination, std::make_index_sequence<limit>{});
        for (size_t i = limit; i < Size; i++)
            destination[i] = source[i];
    }

    template<unsigned UnrollFactor = UnrollFactorSIMD, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline static void copy(const T_data* source, T_data* destination, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _copy<Policy>(source + i, destination + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            destination[i] = source[i];
    }

    template<unsigned Size, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline void copy(const T_data* source, T_data* destination) {
        constexpr auto limit = Size - (Size % SIMDTypeTraits<T, Type>::BlockSize);
        Child::template _copy<Policy>(source, destination, std::make_index_sequence<limit>{});
        for (size_t i = limit; i < Size; i++)
            destination[i] = source[i];
    }

    template<unsigned UnrollFactor = UnrollFactorSIMD, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline static void setValue(T_data *data, T_data value, unsigned size) {
        
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _setValue<Policy>(data + i, value, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            data[i] = value;
    }

    template<unsigned Size, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline void setValue(T_data *data, T_data value) {
        constexpr auto limit = Size - (Size % SIMDTypeTraits<T, Type>::BlockSize);
        Child::template _setValue<Policy>(data, value, std::make_index_sequence<limit>{});
        for (size_t i = limit; i < Size; i++)
            data[i] = value;
    }


    template<unsigned UnrollFactor = UnrollFactorSIMD, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline static void setZero(T_data *data, unsigned size) {
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _setZero<Policy>(data + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            data[i] = 0;
    }

    template<unsigned Size, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline void setZero(T_data *data) {
        constexpr auto limit = Size - (Size % SIMDTypeTraits<T, Type>::BlockSize);
        Child::template _setZero<Policy>(data, std::make_index_sequence<limit>{});
        for (size_t i = limit; i < Size; i++)
            data[i] = 0;
    }

    constexpr inline static bool areEqual(const T_data *a, const T_data *b, unsigned size){
        bool result = true;
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            result = result && Child::template_areEqual(a + i, b + i, std::make_index_sequence<UnrollFactorSIMD>{});
        for (size_t i = limit; i < size; i++)
            result = result && (a[i] == b[i]);
        return result;
    }

    template<unsigned Size>
    constexpr inline bool areEqual(const T_data *a, const T_data *b){
        constexpr auto limit = Size - (Size % SIMDTypeTraits<T, Type>::BlockSize);
        bool result = true;
        result = result && Child::template _areEqual(a, b, std::make_index_sequence<limit>{});
        for (size_t i = limit; i < Size; i++)
            result = result && (a[i] == b[i]);
        return result;
    }
};

// Double Specialization
template<>
struct SIMDMemoryOperations<double, SIMDType::AVX2>
    : public SIMDMemoryOperationsBase<double, SIMDType::AVX2, SIMDMemoryOperations<double, SIMDType::AVX2>> {

    using Base = SIMDMemoryOperationsBase<double, SIMDType::AVX2, SIMDMemoryOperations<double, SIMDType::AVX2>>;
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

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* __restrict source, Base::T_simd* __restrict destination, std::index_sequence<Is...>) {
        ((destination[Is] = _mm256_load_pd(source + Is * Base::registerSize)), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _store(const Base::T_simd* __restrict source, Base::T_data* __restrict destination, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(destination + Is * Base::registerSize, source[Is])), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict source, Base::T_data* __restrict destination, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(destination + Is * Base::registerSize, _mm256_load_pd(source + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict destination, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(destination + Is * Base::registerSize, _mm256_setzero_pd())), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setValue(Base::T_data* __restrict destination, const Base::T_data& value, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(destination + Is * Base::registerSize, _mm256_set1_pd(value))), ...);
    }

    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data* __restrict a, const Base::T_data* __restrict b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result && _mm256_testc_pd(_mm256_load_pd(a + Is * Base::registerSize), _mm256_load_pd(b + Is * Base::registerSize))), ...);
        return result;
    };
};

// Float Specialization
template<>
struct SIMDMemoryOperations<float, SIMDType::AVX2>
    : public SIMDMemoryOperationsBase<float, SIMDType::AVX2, SIMDMemoryOperations<float, SIMDType::AVX2>> {

    using Base = SIMDMemoryOperationsBase<float, SIMDType::AVX2, SIMDMemoryOperations<float, SIMDType::AVX2>>;

private:
    friend Base;

    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm256_store_ps(destination, source);
            else
                _mm256_stream_ps(destination, source);
        }
    };

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_ps(src + Is * Base::registerSize)), ...);
    }
    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _store(const Base::T_simd* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, src[Is])), ...);
    }
    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_load_ps(src + Is * Base::registerSize))), ...);
    }
    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_setzero_ps())), ...);
    }
    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setValue(Base::T_data* dst, const Base::T_data& val, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_set1_ps(val))), ...);
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
struct SIMDMemoryOperations<int, SIMDType::AVX2>
    : public SIMDMemoryOperationsBase<int, SIMDType::AVX2, SIMDMemoryOperations<int, SIMDType::AVX2>> {

    using Base = SIMDMemoryOperationsBase<int, SIMDType::AVX2, SIMDMemoryOperations<int, SIMDType::AVX2>>;

private:
    friend Base;
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
            else
                _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
        }
    };

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _store(const Base::T_simd* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, src[Is])), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize)))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_setzero_si256())), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setValue(Base::T_data* __restrict dst, const Base::T_data& val, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_set1_epi32(val))), ...);
    }

    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data* __restrict a, const Base::T_data* __restrict b, std::index_sequence<Is...>) {
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
struct SIMDMemoryOperations<unsigned, SIMDType::AVX2>
    : public SIMDMemoryOperationsBase<unsigned, SIMDType::AVX2, SIMDMemoryOperations<unsigned, SIMDType::AVX2>> {
    using Base = SIMDMemoryOperationsBase<unsigned, SIMDType::AVX2, SIMDMemoryOperations<unsigned, SIMDType::AVX2>>;
private:
    friend Base;
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
            else
                _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
        }
    };

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _store(const Base::T_simd* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, src[Is])), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize)))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_setzero_si256())), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setValue(Base::T_data* __restrict dst, const Base::T_data& val, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_set1_epi32(val))), ...);
    }

    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data* __restrict a, const Base::T_data* __restrict b, std::index_sequence<Is...>) {
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
struct SIMDMemoryOperations<short, SIMDType::AVX2>
    : public SIMDMemoryOperationsBase<short, SIMDType::AVX2, SIMDMemoryOperations<short, SIMDType::AVX2>> {

    using Base = SIMDMemoryOperationsBase<short, SIMDType::AVX2, SIMDMemoryOperations<short, SIMDType::AVX2>>;

private:

    friend Base;
    template <SIMDStoreType Policy>
    struct StoreFunction {
        static constexpr inline void store(Base::T_data* __restrict destination, const Base::T_simd source) {
            if constexpr (Policy == SIMDStoreType::Cached)
                _mm256_store_si256(reinterpret_cast<__m256i*>(destination), source);
            else
                _mm256_stream_si256(reinterpret_cast<__m256i*>(destination), source);
        }
    };

    template <size_t... Is>
    static constexpr inline void _load(const Base::T_data* src, Base::T_simd* dst, std::index_sequence<Is...>) {
        ((dst[Is] = _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _store(const Base::T_simd* src, Base::T_data* dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, src[Is])), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _copy(const Base::T_data* __restrict src, Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_load_si256(reinterpret_cast<const __m256i*>(src + Is * Base::registerSize)))), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setZero(Base::T_data* __restrict dst, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_setzero_si256())), ...);
    }

    template <SIMDStoreType Policy, size_t... Is>
    static constexpr inline void _setValue(Base::T_data* __restrict dst, const Base::T_data& val, std::index_sequence<Is...>) {
        ((StoreFunction<Policy>::store(dst + Is * Base::registerSize, _mm256_set1_epi16(val))), ...);
    }

    template <size_t... Is>
    static constexpr inline bool _areEqual(const Base::T_data* __restrict a, const Base::T_data* __restrict b, std::index_sequence<Is...>) {
        bool result = true;
        ((result = result &&
            _mm256_testc_si256(
                _mm256_load_si256(reinterpret_cast<const __m256i*>(a + Is * Base::registerSize)),
                _mm256_load_si256(reinterpret_cast<const __m256i*>(b + Is * Base::registerSize))
            )), ...);
        return result;
    }
};

// Unsigned Specialization

#endif //STALKER_MemoryTraits_H
