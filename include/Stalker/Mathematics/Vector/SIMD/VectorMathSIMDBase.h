//
// Created by hal9000 on 7/9/24.
//

#pragma once

#include <utility>
#include <array>
#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDBase.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDBase.h>

namespace Stalker::Mathematics {

    using namespace Stalker::Core;
    using namespace Stalker::Memory;

// CRTP Base
template<typename T, T_SIMD Type, typename Child>
struct VectorMathSIMDBase {
    
    using Traits = TypeTraitsSIMD<T, Type>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;
    using MemoryOps = Stalker::Memory::MemoryOperationsSIMD<T, Type>;
    
public:
    static constexpr unsigned registerSize = TypeTraitsSIMD<T, Type>::RegisterSize();
    
    template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
    inline static void add(size_t size, const T_data *a, const T_data  *b, T_data *result, T_data scaleA, T_data scaleB) {
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD1, scalarSIMD2;
        MemoryOps::broadcast(&scalarSIMD1, scaleA);
        MemoryOps::broadcast(&scalarSIMD2, scaleB);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _add<Policy, true>(a + i, b + i, result  + i, std::make_index_sequence<Unroll>{}, &scalarSIMD1, &scalarSIMD2);
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] * scaleA + b[i] * scaleB;
    }

    template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
    inline static void add(size_t size, const T_data *a, const T_data *b, T_data *result) {
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _add<Policy, false>(a + i, b + i, result + i, std::make_index_sequence<Unroll>{});
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] + b[i];
    }

    template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
    inline static void axpy(size_t size, const T_data *a, const T_data  *b, T_data *result, T_data scale) {
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD;
        MemoryOps::broadcast(&scalarSIMD, scale);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _axpy<Policy>(a + i, b + i, result  + i, std::make_index_sequence<Unroll>{}, &scalarSIMD);
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] * scale + b[i];
    }

    template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
    inline static void subtract(size_t size, const T_data *a, const T_data  *b, T_data *result, T_data scaleA, T_data scaleB) {
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD1, scalarSIMD2;
        MemoryOps::broadcast(&scalarSIMD1, scaleA);
        if constexpr (std::is_floating_point_v<T>)
            MemoryOps::broadcast(&scalarSIMD2, -scaleB);
        else
            MemoryOps::broadcast(&scalarSIMD2, scaleB);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _subtract<Policy, true>(a + i, b + i, result  + i, std::make_index_sequence<Unroll>{}, &scalarSIMD1, &scalarSIMD2);
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] * scaleA - b[i] * scaleB;  
    }

    template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
    inline static void subtract(size_t size, const T_data *a, const T_data *b, T_data *result) {
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _subtract<Policy, false>(a + i, b + i, result + i, std::make_index_sequence<Unroll>{});
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] - b[i];
    }

    template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
    inline static void multiply(size_t size, const T_data *a, const T_data  *b, T_data *result, T_data scaleA, T_data scaleB) {
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD1, scalarSIMD2;
        MemoryOps::broadcast(&scalarSIMD1, scaleA);
        MemoryOps::broadcast(&scalarSIMD2, scaleB);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _multiply<Policy, true>(a + i, b + i, result  + i, std::make_index_sequence<Unroll>{}, &scalarSIMD1, &scalarSIMD2);
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] * scaleA * b[i] * scaleB;
    }

    template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
    inline static void multiply(size_t size, const T_data *a, const T_data *b, T_data *result) {
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _multiply<Policy, false>(a + i, b + i, result + i, std::make_index_sequence<Unroll>{});
        for (size_t i = limit; i < size; i++)
            result[i] = a[i] * b[i];
    }

    template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
    inline static void scale(size_t size, const T_data *data, T_data *result, T scalar){
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD;
        MemoryOps::broadcast(&scalarSIMD, scalar);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _scale<Policy>(data + i, result + i, &scalarSIMD, std::make_index_sequence<Unroll>{});
        for (size_t i = limit; i < size; i++)
            result[i] = data[i] * scalar;
    }

    template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
    inline static void scale(size_t size, __restrict T_data *data, T scalar){
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD;
        MemoryOps::broadcast(&scalarSIMD, scalar);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _scale<Policy>(data + i, &scalarSIMD, std::make_index_sequence<Unroll>{});
        for (size_t i = limit; i < size; i++)
            data[i] *= scalar;
    }

    template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
    inline static void addConstant(size_t size, const T_data *data, T_data *result, T constant) {
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD;
        MemoryOps::broadcast(&scalarSIMD, constant);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _addConstant<Policy>(data + i, result + i, &scalarSIMD, std::make_index_sequence<Unroll>{});
        for (size_t i = limit; i < size; i++)
            result[i] = data[i] + constant;
    }

    template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
    inline static void addConstant(size_t size, __restrict T_data *data, T constant) {
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD;
        MemoryOps::broadcast(&scalarSIMD, constant);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::template _addConstant<Policy>(data + i, &scalarSIMD, std::make_index_sequence<Unroll>{});
        for (size_t i = limit; i < size; i++)
            data[i] += constant;
    }

    template<unsigned Unroll = DefaultUnroll()>
    inline static T_data sum(size_t size, const T* __restrict data) {
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        T_simd accumulators[Unroll];
        for (auto& acc : accumulators) 
        MemoryOps::setZeroRegister(&acc);
        for (size_t i = 0; i < limit; i += blockSize)
            Child::_sum(data + i, accumulators , std::make_index_sequence<Unroll>{});
        T_data result = _registerSum(accumulators, Unroll);
        for (size_t i = limit; i < size; i++)
            result += data[i];
        return result;
    }

    template<unsigned Unroll = DefaultUnroll()>
    inline static T_data dot(size_t size, const T* __restrict a, const T* __restrict b) {
        constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
        auto limit = size - (size % blockSize);
        T_simd accumulators[Unroll];
        for (auto& acc : accumulators) 
        MemoryOps::setZeroRegister(&acc);
        for (size_t i = 0; i < limit; i += blockSize)
        Child::_dot(a + i, b + i, accumulators, std::make_index_sequence<Unroll>{});
        T_data result = _registerSum(accumulators, Unroll);
        for (size_t i = limit; i < size; i++)
            result += (a[i] * b[i]);
        return result;
    }
protected:
    template <size_t Index>
    static constexpr inline size_t _registerOffset() { return Index * Traits::RegisterSize(); }

private:
    inline static T _registerSum(const T_simd* __restrict data, size_t size) {
        T_data result = 0;
        for (unsigned i = 0; i < size; ++i) {
            alignas(64) T_data temp[Traits::RegisterSize()];
            MemoryOps::template store<T_SIMDStore::Cached>(temp, data[i]);
            for (unsigned j = 0; j < Traits::RegisterSize(); ++j) {
                result += temp[j];
            }
        }
        return result;
    }
};

template<typename T, T_SIMD Type> struct VectorMathSIMD; 


} // namespace Stalker::Mathematics

