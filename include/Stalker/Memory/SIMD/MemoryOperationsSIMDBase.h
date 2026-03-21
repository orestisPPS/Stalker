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

//
// Created by hal9000 on 7/8/24.
//
#pragma once
#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDBase.h>
#include <Stalker/Core/Traits/ExecutionTraits.h>
#include <Stalker/Memory/Prefetcher.h>

namespace Stalker::Memory {

using namespace Stalker::Core;

    template<typename T, T_SIMD Type, typename Child>
    struct MemoryOperationsSIMDBase {
        
        using Traits = TypeTraitsSIMD<T, Type>;
        using T_simd = typename Traits::typeSIMD;
        using T_data = typename Traits::typeData;
        
    public:

        template<typename ExecTrait>
        STALKER_FORCE_INLINE static void copy(size_t size, T_data* STALKER_RESTRICT destination, const T_data* STALKER_RESTRICT source ) {
            constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
            auto limit = size - (size % blockSize);
            for (size_t i = 0; i < limit; i += blockSize) {
                if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone) {
                    Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(destination + i + blockSize);
                }
                _copy<ExecTrait::IsAligned, ExecTrait::StorePolicy, ExecTrait::ILPPolicy>(source + i, destination + i, UnrollIndexSequence<ExecTrait::Unroll>{});
            }
            for (size_t i = limit; i < size; ++i)
                destination[i] = source[i];
        }

        template<typename ExecTrait>
        STALKER_FORCE_INLINE static void swap(size_t size, T_data* STALKER_RESTRICT data1, T_data* STALKER_RESTRICT data2) {
            constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
            auto limit = size - (size % blockSize);
            for (size_t i = 0; i < limit; i += blockSize) {
                if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone) {
                    Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(data1 + i + blockSize);
                    Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(data2 + i + blockSize);
                }
                _swap<ExecTrait::IsAligned, ExecTrait::StorePolicy, ExecTrait::ILPPolicy>(data1 + i, data2 + i, UnrollIndexSequence<ExecTrait::Unroll>{});
            }
            for (size_t i = limit; i < size; ++i)
                std::swap(data1[i], data2[i]);
        }
        
        template<typename ExecTrait>
        STALKER_FORCE_INLINE static void setValue(size_t size, T_data* STALKER_RESTRICT data, T_data value) {
            constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
            auto limit = size - (size % blockSize);
            T_simd scalarSIMD;
            Child::template _broadcast(&scalarSIMD, value, UnrollIndexSequence<1>{});
            for (size_t i = 0; i < limit; i += blockSize)
                _setValue<ExecTrait::IsAligned, ExecTrait::StorePolicy>(data + i, scalarSIMD, UnrollIndexSequence<ExecTrait::Unroll>{});
            for (size_t i = limit; i < size; i++)
                data[i] = value;
        }

        template<typename ExecTrait>
        STALKER_FORCE_INLINE static bool areEqual(size_t size, const T_data* a, const T_data *b){
            constexpr unsigned blockSize = Traits::template BlockSize<DefaultUnroll()>();
            auto limit = size - (size % blockSize);
            bool result = true;
            for (size_t i = 0; i < limit; i += blockSize)
                result = result && _areEqualUnrolled<ExecTrait::IsAligned>(a + i, b + i, UnrollIndexSequence<DefaultUnroll()>{});
            for (size_t i = limit; i < size; i++)
                result = result && (a[i] == b[i]);
            return result;
        }

        template <size_t Index, bool IsAligned = false>
        STALKER_FORCE_INLINE static T_simd loadOffset(const T_data* STALKER_RESTRICT data) {
            return Child::template _load<IsAligned>(data + Index * Traits::RegisterSize());
        }
        template <size_t Index, bool IsAligned = false>
        STALKER_FORCE_INLINE static T_simd load(const T_data* STALKER_RESTRICT data) {
            return Child::template _load<IsAligned>(data + Index * Traits::RegisterSize());
        }
        
        template<T_SIMDStore Policy = DefaultSIMDStore(), bool IsAligned = false>
        STALKER_FORCE_INLINE static void store(T_data* STALKER_RESTRICT destination, const T_simd& source) {
            Child::template _store<Policy, IsAligned>(destination, source);
        }

        template<size_t Index, T_SIMDStore Policy = DefaultSIMDStore(), bool IsAligned = false>
        STALKER_FORCE_INLINE static void storeOffset(T_data* STALKER_RESTRICT destination, const T_simd& source) {
            Child::template _store<Policy, IsAligned>(destination + Index * Traits::RegisterSize(), source);
        }
        
        STALKER_FORCE_INLINE static void zeroRegister(T_simd* STALKER_RESTRICT destination) {
            Child::_setZeroRegister(destination);
        }

        STALKER_FORCE_INLINE static T_simd zeroRegister() {
            return Child::_setZeroRegister();
        }

        template<unsigned Unroll = 1>
        STALKER_FORCE_INLINE static void broadcast(T_simd* STALKER_RESTRICT destination, T scalar) {
            Child::_broadcast(destination, scalar, UnrollIndexSequence<Unroll>{});
        }

        STALKER_FORCE_INLINE static T_simd broadcast(T scalar) {
            return Child::_broadcast(scalar);
        }

    protected:

        template <size_t Index>
        STALKER_FORCE_INLINE constexpr static size_t _registerOffset() { return Index * Traits::RegisterSize(); }

        template <bool IsAligned, size_t... Is>
        STALKER_FORCE_INLINE static bool _areEqualUnrolled(const T_data*  a, const T_data* b, std::index_sequence<Is...>) {
            bool result = true;
            ((result = result && Child::_areEqual(loadOffset<Is, IsAligned>(a), loadOffset<Is, IsAligned>(b))), ...);
            return result;
        }

        template <bool IsAligned, T_SIMDStore Policy, T_ILPPolicy ILP, size_t... Is>
        STALKER_FORCE_INLINE static void _copy(const T_data* STALKER_RESTRICT source, T_data* STALKER_RESTRICT destination, std::index_sequence<Is...>) {
            if constexpr (ILP == T_ILPPolicy::Interleaved) {
                ((storeOffset<Is, Policy, IsAligned>(destination, loadOffset<Is, IsAligned>(source))), ...);
            } else if constexpr (ILP == T_ILPPolicy::Grouped) {
                const T_simd loadedSource[] = { loadOffset<Is, IsAligned>(source)... };
                ((storeOffset<Is, Policy, IsAligned>(destination, loadedSource[Is])), ...);
            }
        }

        template <bool IsAligned, T_SIMDStore Policy, T_ILPPolicy ILP, size_t... Is>
        STALKER_FORCE_INLINE static void _swap(T_data* STALKER_RESTRICT data1, T_data* STALKER_RESTRICT data2, std::index_sequence<Is...>) {
            if constexpr (ILP == T_ILPPolicy::Interleaved) {
                (([] (T_data* d1, T_data* d2) {
                    T_simd loadedD1 = loadOffset<Is, IsAligned>(d1);
                    storeOffset<Is, Policy, IsAligned>(d1, loadOffset<Is, IsAligned>(d2));
                    storeOffset<Is, Policy, IsAligned>(d2, loadedD1);
                })(data1, data2), ...);
            } else if constexpr (ILP == T_ILPPolicy::Grouped) {
                const T_simd loadedD1[] = { loadOffset<Is, IsAligned>(data1)... };
                const T_simd loadedD2[] = { loadOffset<Is, IsAligned>(data2)... };
                ((storeOffset<Is, Policy, IsAligned>(data1, loadedD2[Is])), ...);
                ((storeOffset<Is, Policy, IsAligned>(data2, loadedD1[Is])), ...);
            }
        }

        template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
        STALKER_FORCE_INLINE static void _setValue(T_data* STALKER_RESTRICT data, const T_simd& scalarSIMD, std::index_sequence<Is...>) {
            (storeOffset<Is, Policy, IsAligned>(data, scalarSIMD), ... );
        }
        
    };

    template<typename T, T_SIMD Type> struct MemoryOperationsSIMD; 

} // namespace Stalker::Memory