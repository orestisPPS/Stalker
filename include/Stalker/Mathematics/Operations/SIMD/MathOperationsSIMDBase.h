//
// Created by hal9000 on 7/9/24.
//

#pragma once

#include <utility>
#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDBase.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDBase.h>

namespace Stalker::Mathematics::SIMD {

    using namespace Stalker::Core;
    using namespace Stalker::Memory::SIMD;
    using SIMDType::AVX2;
    using SIMDType::AVX512;

// CRTP Base
template<typename T, SIMDType Type, typename Child>
struct MathOperationsSIMDBase {
    
    using Traits = TypeTraitsSIMD<T, Type>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;
    using MemoryOps = Stalker::Memory::SIMD::MemoryOperationsSIMD<T, Type>;
    
public:
    static constexpr unsigned registerSize = TypeTraitsSIMD<T, Type>::RegisterSize();
    
    template<unsigned UnrollFactor = DefaultUnrollFactor(), SIMDStoreType Policy = DefaultSIMDStoreType()>
    inline static void add(size_t size, const T_data *a, const T_data  *b, T_data *result, T_data scaleA, T_data scaleB) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD1, scalarSIMD2;
        MemoryOps::broadcast(&scalarSIMD1, scaleA);
        MemoryOps::broadcast(&scalarSIMD2, scaleB);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _add<Policy, true>(a + i, b + i, result  + i, std::make_index_sequence<UnrollFactor>{}, &scalarSIMD1, &scalarSIMD2);
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] * scaleA + b[i] * scaleB;
    }

    template<unsigned UnrollFactor = DefaultUnrollFactor(), SIMDStoreType Policy = DefaultSIMDStoreType()>
    inline static void add(size_t size, const T_data *a, const T_data *b, T_data *result) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _add<Policy, false>(a + i, b + i, result + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] + b[i];
    }

    template<unsigned UnrollFactor = DefaultUnrollFactor(), SIMDStoreType Policy = DefaultSIMDStoreType()>
    inline static void subtract(size_t size, const T_data *a, const T_data  *b, T_data *result, T_data scaleA, T_data scaleB) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD1, scalarSIMD2;
        MemoryOps::broadcast(&scalarSIMD1, scaleA);
        if constexpr (std::is_floating_point_v<T>)
            MemoryOps::broadcast(&scalarSIMD2, -scaleB);
        else
            MemoryOps::broadcast(&scalarSIMD2, scaleB);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _subtract<Policy, true>(a + i, b + i, result  + i, std::make_index_sequence<UnrollFactor>{}, &scalarSIMD1, &scalarSIMD2);
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] * scaleA - b[i] * scaleB;  
    }

    template<unsigned UnrollFactor = DefaultUnrollFactor(), SIMDStoreType Policy = DefaultSIMDStoreType()>
    inline static void subtract(size_t size, const T_data *a, const T_data *b, T_data *result) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _subtract<Policy, false>(a + i, b + i, result + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] - b[i];
    }

    template<unsigned UnrollFactor = DefaultUnrollFactor(), SIMDStoreType Policy = DefaultSIMDStoreType()>
    inline static void multiply(size_t size, const T_data *a, const T_data  *b, T_data *result, T_data scaleA, T_data scaleB) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD1, scalarSIMD2;
        MemoryOps::broadcast(&scalarSIMD1, scaleA);
        MemoryOps::broadcast(&scalarSIMD2, scaleB);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _multiply<Policy, true>(a + i, b + i, result  + i, std::make_index_sequence<UnrollFactor>{}, &scalarSIMD1, &scalarSIMD2);
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] * scaleA * b[i] * scaleB;
    }

    template<unsigned UnrollFactor = DefaultUnrollFactor(), SIMDStoreType Policy = DefaultSIMDStoreType()>
    inline static void multiply(size_t size, const T_data *a, const T_data *b, T_data *result) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _multiply<Policy, false>(a + i, b + i, result + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] * b[i];
    }

    template<unsigned UnrollFactor = DefaultUnrollFactor(), SIMDStoreType Policy = DefaultSIMDStoreType()>
    inline static void scale(size_t size, const T_data *data, T_data *result, T scalar){
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD;
        MemoryOps::broadcast(&scalarSIMD, scalar);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::_scale(data + i, result + i, scalarSIMD, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            result[i] = data[i] * scalar;
    }

    template<unsigned UnrollFactor = DefaultUnrollFactor(), SIMDStoreType Policy = DefaultSIMDStoreType()>
    inline static void scale(size_t size, __restrict T_data *data, T scalar){
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD;
        MemoryOps::broadcast(&scalarSIMD, scalar);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::_scale(data + i, data + i, scalarSIMD, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            data[i] *= scalar;
    }

    template<unsigned UnrollFactor = DefaultUnrollFactor(), SIMDStoreType Policy = DefaultSIMDStoreType()>
    inline static void addConstant(size_t size, const T_data *data, T_data *result, T constant) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD;
        MemoryOps::broadcast(&scalarSIMD, constant);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::_addConstant(data + i, result + i, scalarSIMD, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            result[i] = data[i] + constant;
    }

    template<unsigned UnrollFactor = DefaultUnrollFactor(), SIMDStoreType Policy = DefaultSIMDStoreType()>
    inline static void addConstant(size_t size, __restrict T_data *data, T constant) {
        constexpr unsigned blockSize = Traits::template BlockSize<UnrollFactor>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD;
        MemoryOps::broadcast(&scalarSIMD, constant);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::_addConstant(data + i, scalarSIMD, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; i++)
            data[i] += constant;
    }
};

template<typename T, SIMDType Type> struct MathOperationsSIMD; 


} // namespace Stalker::Mathematics

