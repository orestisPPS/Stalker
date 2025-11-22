//
// Created by hal9000 on 7/8/24.
//
#pragma once
#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDBase.h>

namespace Stalker::Memory {

using namespace Stalker::Core;

    template<typename T, T_SIMD Type, typename Child>
    struct MemoryOperationsSIMDBase {
        
        using Traits = TypeTraitsSIMD<T, Type>;
        using T_simd = typename Traits::typeSIMD;
        using T_data = typename Traits::typeData;
        
    public:

        template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
        constexpr inline static void copy(size_t size, T_data* __restrict destination, const T_data* __restrict source ) {
            constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
            auto limit = size - (size % blockSize);
            for (size_t i = 0; i < limit; i += blockSize)
                Child::template _copy<Policy>(source + i, destination + i, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; i++)
                destination[i] = source[i];
        }
        
        template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
        constexpr inline static void setValue(size_t size, T_data* __restrict data, T_data value) {
            constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
            auto limit = size - (size % blockSize);
            T_simd scalarSIMD;
            broadcast(&scalarSIMD, value);
            for (size_t i = 0; i < limit; i += blockSize)
                Child::template _setValue<Policy>(data + i, &scalarSIMD, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; i++)
                data[i] = value;
        }
        
        template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
        constexpr inline static void setZero(size_t size, T_data* __restrict data) {
            constexpr unsigned blockSize = Traits::template BlockSize<Unroll>();
            auto limit = size - (size % blockSize);
            for (size_t i = 0; i < limit; i += blockSize)
                Child::template _setZero<Policy>(data + i, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; i++)
                data[i] = 0;
        }

        template<unsigned Unroll = DefaultUnroll(), T_SIMDStore Policy = DefaultSIMDStore()>
        constexpr inline static bool areEqual(size_t size, const T_data* a, const T_data *b){
            constexpr unsigned blockSize = Traits::template BlockSize<DefaultUnroll()>();
            auto limit = size - (size % blockSize);
            bool result = true;
            for (size_t i = 0; i < limit; i += blockSize)
                result = result && Child::template_areEqual(a + i, b + i, std::make_index_sequence<DefaultUnroll()>{});
            for (size_t i = limit; i < size; i++)
                result = result && (a[i] == b[i]);
            return result;
        }
        
        template <bool IsAligned = false>
        constexpr inline static T_simd load(const T_data* __restrict data) {
            return Child::_load(data);
        }
        
        template<T_SIMDStore Policy = DefaultSIMDStore()>
        constexpr inline static void store(T_data* __restrict destination, const T_simd& source) {
            Child::template _store<Policy>(destination, source);
        }
        
        inline static void setZeroRegister(T_simd* __restrict destination) {
            Child::_setZeroRegister(destination);
        }

        template<unsigned Unroll = 1>
        constexpr inline static void broadcast(T_simd* __restrict destination, T scalar) {
            Child::_broadcast(destination, scalar, std::make_index_sequence<Unroll>{});
        }

    protected:

        template <size_t Index>
        static constexpr inline size_t _registerOffset() { return Index * Traits::RegisterSize(); }
        
    };

    template<typename T, T_SIMD Type> struct MemoryOperationsSIMD; 

} // namespace Stalker::Memory