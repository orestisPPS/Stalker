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
// Created by hal9000 on 7/9/24.
//

#pragma once

#include <cmath>
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
    
    template<typename ExecTrait>
    STALKER_FORCE_INLINE static void add(size_t size, const T_data *a, const T_data  *b, T_data *result, T_data scaleA, T_data scaleB) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD1, scalarSIMD2;
        MemoryOps::broadcast(&scalarSIMD1, scaleA);
        MemoryOps::broadcast(&scalarSIMD2, scaleB);
        auto kernel = [scalarSIMD1, scalarSIMD2](const T_simd &regA, const T_simd &regB) STALKER_FORCE_INLINE {
            return Child::_axpy(regA, Child::_multiply(regB, scalarSIMD2), scalarSIMD1);
        };

        for (size_t i = 0; i < limit; i += blockSize){
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone) {
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(a + i + blockSize);
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(b + i + blockSize);
            }
            _unrollBinary<ExecTrait>(kernel, a + i, b + i, result + i, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        for (size_t i = limit; i < size; ++i)
            result[i] = a[i] * scaleA + b[i] * scaleB;
    }

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static void add(size_t size, const T_data *a, const T_data *b, T_data *result) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        auto kernel = [](const T_simd &regA, const T_simd &regB) STALKER_FORCE_INLINE {
            return Child::_add(regA, regB);
        };

        for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone) {
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(a + i + blockSize);
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(b + i + blockSize);
            }
            _unrollBinary<ExecTrait>(kernel, a + i, b + i, result + i, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        for (size_t i = limit; i < size; ++i)
            result[i] = a[i] + b[i];
    }

    STALKER_FORCE_INLINE static T_simd addRegister(const T_simd& a, const T_simd& b) {
        return Child::_add(a, b);
    }

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static void axpy(size_t size, const T_data *a, const T_data  *b, T_data *result, T_data scale) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarReg;
        MemoryOps::broadcast(&scalarReg, scale);
        auto kernel = [scalarReg](const T_simd &regA, const T_simd &regB) STALKER_FORCE_INLINE {
            return Child::_axpy(regA, regB, scalarReg);
        };
         for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone) {
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(a + i + blockSize);
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(b + i + blockSize);
            }
            _unrollBinary<ExecTrait>(kernel, a + i, b + i, result + i, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        for (size_t i = limit; i < size; ++i)
            result[i] = a[i] * scale + b[i];
    }

    STALKER_FORCE_INLINE static T_simd axpyRegister(const T_simd& a, const T_simd& b, const T_simd& scalar) {
        return Child::_axpy(a, b, scalar);
    }

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static void subtract(size_t size, const T_data *a, const T_data  *b, T_data *result, T_data scaleA, T_data scaleB) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD1, scalarSIMD2;
        MemoryOps::broadcast(&scalarSIMD1, scaleA);
        MemoryOps::broadcast(&scalarSIMD2, scaleB);
        auto kernel = [scalarSIMD1, scalarSIMD2](const T_simd &regA, const T_simd &regB) STALKER_FORCE_INLINE {
            return Child::_axmy(regA, Child::_multiply(regB, scalarSIMD2), scalarSIMD1);
        };
        for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone) {
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(a + i + blockSize);
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(b + i + blockSize);
            }
            _unrollBinary<ExecTrait>(kernel, a + i, b + i, result + i, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        for (size_t i = limit; i < size; ++i)
            result[i] = a[i] * scaleA - b[i] * scaleB;
    }

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static void subtract(size_t size, const T_data *a, const T_data *b, T_data *result) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        auto kernel = [](const T_simd &regA, const T_simd &regB) STALKER_FORCE_INLINE {
            return Child::_subtract(regA, regB);
        };
         for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone) {
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(a + i + blockSize);
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(b + i + blockSize);
            }
            _unrollBinary<ExecTrait>(kernel, a + i, b + i, result + i, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        for (size_t i = limit; i < size; ++i)
            result[i] = a[i] - b[i];
    }
    
    STALKER_FORCE_INLINE static T_simd subtractRegister(const T_simd& a, const T_simd& b) {
        return Child::_subtract(a, b);
    }   

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static void multiply(size_t size, const T_data *a, const T_data  *b, T_data *result, T_data scaleA, T_data scaleB) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarSIMD1, scalarSIMD2;
        MemoryOps::broadcast(&scalarSIMD1, scaleA);
        MemoryOps::broadcast(&scalarSIMD2, scaleB);
        auto kernel = [scalarSIMD1, scalarSIMD2](const T_simd &regA, const T_simd &regB) STALKER_FORCE_INLINE {
            return Child::_multiply(Child::_multiply(regA, scalarSIMD1), Child::_multiply(regB, scalarSIMD2));
        };
        for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone) {
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(a + i + blockSize);
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(b + i + blockSize);
            }
            _unrollBinary<ExecTrait>(kernel, a + i, b + i, result + i, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        for (size_t i = limit; i < size; ++i)
            result[i] = (a[i] * scaleA) * (b[i] * scaleB);
    }

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static void multiply(size_t size, const T_data *a, const T_data *b, T_data *result) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        auto kernel = [](const T_simd &regA, const T_simd &regB) STALKER_FORCE_INLINE {
            return Child::_multiply(regA, regB);
        };
         for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone) {
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(a + i + blockSize);
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(b + i + blockSize);
            }
            _unrollBinary<ExecTrait>(kernel, a + i, b + i, result + i, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        for (size_t i = limit; i < size; ++i)
            result[i] = a[i] * b[i];
    }

    STALKER_FORCE_INLINE static T_simd multiplyRegister(const T_simd& a, const T_simd& b) {
        return Child::_multiply(a, b);
    }

    STALKER_FORCE_INLINE static T_simd axmyRegister(const T_simd& a, const T_simd& b, const T_simd& scalar) {
        return Child::_axmy(a, b, scalar);
    }

    template<typename ExecTrait> 
    STALKER_FORCE_INLINE static void scale(size_t size, const T_data * STALKER_RESTRICT data, T_data * STALKER_RESTRICT result, T scalar){
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarReg;
        MemoryOps::broadcast(&scalarReg, scalar);
        auto kernel = [scalarReg](const T_simd &value) STALKER_FORCE_INLINE {
            return Child::_multiply(value, scalarReg);
        };
         for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone){
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(data + i + blockSize);
            }
            _unrollUnary<ExecTrait>(kernel, data + i, result + i, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        for (size_t i = limit; i < size; ++i)
            result[i] = data[i] * scalar;
    }

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static void scale(size_t size, T_data * STALKER_RESTRICT data, T scalar){
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarReg;
        MemoryOps::broadcast(&scalarReg, scalar);
        auto kernel = [scalarReg](const T_simd &value) STALKER_FORCE_INLINE {
            return Child::_multiply(value, scalarReg);
        };
         for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone){
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(data + i + blockSize);
            }
            _unrollUnaryIntoThis<ExecTrait>(kernel, data + i, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        for (size_t i = limit; i < size; ++i)
            data[i] *= scalar;
    }

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static void addConstant(size_t size, const T_data * STALKER_RESTRICT data, T_data * STALKER_RESTRICT result, T constant) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarReg;
        MemoryOps::broadcast(&scalarReg, constant);
        auto kernel = [scalarReg](const T_simd &value) STALKER_FORCE_INLINE {
            return Child::_add(value, scalarReg);
        };
        for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone){
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(data + i + blockSize);
            }
            _unrollUnary<ExecTrait>(kernel, data + i, result + i, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        for (size_t i = limit; i < size; ++i)
            result[i] = data[i] + constant;
    }

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static void addConstant(size_t size, T_data * STALKER_RESTRICT data, T constant) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        T_simd scalarReg;
        MemoryOps::broadcast(&scalarReg, constant);
        auto kernel = [scalarReg](const T_simd &value) STALKER_FORCE_INLINE {
            return Child::_add(value, scalarReg);
        };
        for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone){
                Prefetcher::prefetch<T_data, ExecTrait::PrefetchHint, 1>(data + i + blockSize);
            }
            _unrollUnaryIntoThis<ExecTrait>(kernel, data + i, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        for (size_t i = limit; i < size; ++i)
            data[i] += constant;
    }

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static T_data sum(size_t size, const T* STALKER_RESTRICT data) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        T_simd accumulators[ExecTrait::Unroll];
        for (auto& acc : accumulators) 
            MemoryOps::zeroRegister(&acc);

        auto kernel = [](T_simd &acc, const T_simd &value) STALKER_FORCE_INLINE {
            acc = Child::_add(acc, value);
        };

        for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone){
                Prefetcher::prefetch<T, ExecTrait::PrefetchHint, 1>(data + i + blockSize);
            }
            _unrollUnaryReduced<ExecTrait>(kernel, data + i, accumulators, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        T_data result = _registerSum(accumulators, ExecTrait::Unroll);
        for (size_t i = limit; i < size; ++i)
            result += data[i];
        return result;
    }

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static T_data dot(size_t size, const T* STALKER_RESTRICT a, const T* STALKER_RESTRICT b) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        T_simd accumulators[ExecTrait::Unroll];
        for (auto& acc : accumulators) 
            MemoryOps::zeroRegister(&acc);
        
        auto kernel = [](T_simd &acc, const T_simd &regA, const T_simd &regB) STALKER_FORCE_INLINE {
            acc = Child::_axpy(regA, acc, regB);
        };

        for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone) {
                Prefetcher::prefetch<T, ExecTrait::PrefetchHint, 1>(a + i + blockSize);
                Prefetcher::prefetch<T, ExecTrait::PrefetchHint, 1>(b + i + blockSize);
            }
            _unrollBinaryReduced<ExecTrait>(kernel, a + i, b + i, accumulators, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        T_data result = _registerSum(accumulators, ExecTrait::Unroll);
        for (size_t i = limit; i < size; ++i)
            result += a[i] * b[i];
        return result;
    }

    template<typename ExecTrait>
    STALKER_FORCE_INLINE static T_data sumOfSquares(size_t size, const T* STALKER_RESTRICT data) {
        constexpr unsigned blockSize = Traits::template BlockSize<ExecTrait::Unroll>();
        auto limit = size - (size % blockSize);
        T_simd accumulators[ExecTrait::Unroll];
        for (auto& acc : accumulators) 
            MemoryOps::zeroRegister(&acc);

        auto kernel = [](T_simd &acc, const T_simd &value) STALKER_FORCE_INLINE {
            acc = Child::_axpy(value, acc, value);
        };

        for (size_t i = 0; i < limit; i += blockSize) {
            if constexpr (ExecTrait::PrefetchHint != T_PrefetchHints::HintNone) {
                Prefetcher::prefetch<T, ExecTrait::PrefetchHint, 1>(data + i + blockSize);
            }
            _unrollUnaryReduced<ExecTrait>(kernel, data + i, accumulators, UnrollIndexSequence<ExecTrait::Unroll>{});
        }
        T_data result = _registerSum(accumulators, ExecTrait::Unroll);
        for (size_t i = limit; i < size; ++i)
            result += data[i] * data[i];
        return std::sqrt(result);
    }

protected:
    template <size_t Index>
    static constexpr STALKER_FORCE_INLINE size_t _registerOffset() { return Index * Traits::RegisterSize(); }

private:

    template<typename Trait, typename KernelFn, size_t... Is>
    constexpr STALKER_FORCE_INLINE static void _unrollBinary(KernelFn &&kernel, const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>) {
        if constexpr (Trait::ILPPolicy == T_ILPPolicy::Grouped) {
            const T_simd loadedA[] = {MemoryOps::template loadOffset<Is, Trait::IsAligned>(a)...};
            const T_simd loadedB[] = {MemoryOps::template loadOffset<Is, Trait::IsAligned>(b)...};
            (MemoryOps::template storeOffset<Is, Trait::StorePolicy>(result, kernel(loadedA[Is], loadedB[Is])), ...);
        }
        else if constexpr (Trait::ILPPolicy == T_ILPPolicy::Interleaved) {
            (MemoryOps::template storeOffset<Is, Trait::StorePolicy>(result, kernel(MemoryOps::template loadOffset<Is, Trait::IsAligned>(a),
                                                                                    MemoryOps::template loadOffset<Is, Trait::IsAligned>(b))), ...);
        }
    }

    template<typename Trait, typename KernelFn, size_t... Is>
    constexpr STALKER_FORCE_INLINE static void _unrollBinaryReduced(KernelFn &&kernel, const T_data *a, const T_data *b, T_simd *accumulators, std::index_sequence<Is...>) {
        if constexpr (Trait::ILPPolicy == T_ILPPolicy::Grouped) {
            const T_simd loadedA[] = {MemoryOps::template loadOffset<Is, Trait::IsAligned>(a)...};
            const T_simd loadedB[] = {MemoryOps::template loadOffset<Is, Trait::IsAligned>(b)...};
            (kernel(accumulators[Is], loadedA[Is], loadedB[Is]), ...);
        }
        else if constexpr (Trait::ILPPolicy == T_ILPPolicy::Interleaved) {
            (kernel(accumulators[Is], MemoryOps::template loadOffset<Is, Trait::IsAligned>(a), MemoryOps::template loadOffset<Is, Trait::IsAligned>(b)), ...);
        }
    }

    template<typename Trait, typename KernelFn, size_t... Is>
    constexpr STALKER_FORCE_INLINE static void _unrollUnary(KernelFn &&kernel, const T_data *data, T_data *result, std::index_sequence<Is...>) {
        if constexpr (Trait::ILPPolicy == T_ILPPolicy::Grouped) {
            const T_simd loaded[] = {MemoryOps::template loadOffset<Is, Trait::IsAligned>(data)...};
            const T_simd computed[] = {kernel(loaded[Is])...};
            (MemoryOps::template storeOffset<Is, Trait::StorePolicy>(result, computed[Is]), ...);
        }
        else if constexpr (Trait::ILPPolicy == T_ILPPolicy::Interleaved) {
            (MemoryOps::template storeOffset<Is, Trait::StorePolicy>(result, kernel(MemoryOps::template loadOffset<Is, Trait::IsAligned>(data))), ...);
        }
    }

    template<typename Trait, typename KernelFn, size_t... Is>
    constexpr STALKER_FORCE_INLINE static void _unrollUnaryIntoThis(KernelFn &&kernel, T_data STALKER_RESTRICT *data, std::index_sequence<Is...>) {
        if constexpr (Trait::ILPPolicy == T_ILPPolicy::Grouped) {
            const T_simd loaded[] = {MemoryOps::template loadOffset<Is, Trait::IsAligned>(data)...};
            (MemoryOps::template storeOffset<Is, Trait::StorePolicy>(data, kernel(loaded[Is])), ...);
        }
        else if constexpr (Trait::ILPPolicy == T_ILPPolicy::Interleaved) {
            (MemoryOps::template storeOffset<Is, Trait::StorePolicy>(data, kernel(MemoryOps::template loadOffset<Is, Trait::IsAligned>(data))), ...);
        }
    }

    template<typename Trait, typename KernelFn, size_t... Is>
    constexpr STALKER_FORCE_INLINE static void _unrollUnaryReduced(KernelFn &&kernel, const T_data *data, T_simd *accumulators, std::index_sequence<Is...>) {
        if constexpr (Trait::ILPPolicy == T_ILPPolicy::Grouped) {
            const T_simd loaded[] = {MemoryOps::template loadOffset<Is, Trait::IsAligned>(data)...};
            ((kernel(accumulators[Is], loaded[Is])), ...);
        }
        else if constexpr (Trait::ILPPolicy == T_ILPPolicy::Interleaved) {
            ((kernel(accumulators[Is], MemoryOps::template loadOffset<Is, Trait::IsAligned>(data))), ...);
        }
    }

    STALKER_FORCE_INLINE static T _registerSum(const T_simd* STALKER_RESTRICT data, size_t size) {
        if (size == 0)
            return static_cast<T_data>(0);

        T_simd total = data[0];
        for (unsigned i = 1; i < size; ++i)
            total = Child::_add(total, data[i]);

        alignas(64) T_data temp[Traits::RegisterSize()];
        MemoryOps::template store<true, T_SIMDStore::Cached>(temp, total);

        T_data result = 0;
        for (unsigned j = 0; j < Traits::RegisterSize(); ++j)
            result += temp[j];
        return result;
    }
};

template<typename T, T_SIMD Type> struct VectorMathSIMD; 


} // namespace Stalker::Mathematics

