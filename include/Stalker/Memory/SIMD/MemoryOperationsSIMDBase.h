//
// Created by hal9000 on 7/8/24.
//
#pragma once
#include <utility>
#include <Stalker/Core/Traits/SIMD/SIMDTraitsBase.h>

namespace Stalker::Memory::SIMD {

enum class SIMDStoreType {
    Cached,
    Streamed
};

using namespace Stalker::Core;
// Forward declarations for specialized MemoryOperationsSIMD:

template<typename T, SIMDType Type, typename Child>
struct MemoryOperationsSIMDBase {
    
    using Traits = SIMDTypeTraits<T, Type>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;
    
    public:
    static constexpr unsigned registerSize = Traits::RegisterSize();
    
    template<unsigned UnrollFactor = STALKER_UNROLL_FACTOR>
    constexpr inline static void load(const T_data* __restrict source, T_simd* __restrict destination, unsigned size) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _load(source + i, destination + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
                destination[i] = source[i];
    }
    
    template<unsigned UnrollFactor = STALKER_UNROLL_FACTOR, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline static void store(const T_simd* __restrict source, T_data* __restrict destination) {
        Child::template _store<Policy>(source, destination, std::make_index_sequence<UnrollFactor>{});
    }
    
    template<unsigned UnrollFactor = STALKER_UNROLL_FACTOR, SIMDStoreType Policy = SIMDStoreType::Cached>
    constexpr inline static void copy(const T_data* __restrict source, T_data* __restrict destination, unsigned size) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _copy<Policy>(source + i, destination + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            destination[i] = source[i];
    }
    
    template<unsigned UnrollFactor = STALKER_UNROLL_FACTOR, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline static void setValue(T_data* __restrict data, T_data value, unsigned size) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _setValue<Policy>(data + i, value, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            data[i] = value;
    }
    
    template<unsigned UnrollFactor = STALKER_UNROLL_FACTOR, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline static void setZero(T_data* __restrict data, unsigned size) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _setZero<Policy>(data + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            data[i] = 0;
    }

    template<unsigned UnrollFactor = 1>
    constexpr inline static void broadcast(T_simd* __restrict destination, T scalar) {
        Child::_broadcast(destination, scalar, std::make_index_sequence<UnrollFactor>{});
    }

    template<unsigned UnrollFactor = STALKER_UNROLL_FACTOR, SIMDStoreType Policy = SIMDStoreType::Streamed>
    constexpr inline static bool areEqual(const T_data* a, const T_data *b, unsigned size){
        bool result = true;
        constexpr unsigned blockSize = Traits::template BlockSize<STALKER_UNROLL_FACTOR>();
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            result = result && Child::template_areEqual(a + i, b + i, std::make_index_sequence<STALKER_UNROLL_FACTOR>{});
        for (size_t i = limit; i < size; i++)
            result = result && (a[i] == b[i]);
        return result;
    }
    
};

template<typename T, SIMDType Type> struct MemoryOperationsSIMD; 

// Double Specialization


// Unsigned Specialization

} // namespace Stalker::Memory