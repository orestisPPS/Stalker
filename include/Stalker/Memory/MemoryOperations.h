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

#include <Stalker/Core/Traits/ExecutionTraits.h>
#include <Stalker/Memory/MemoryOperationsClassic.h>
#include <Stalker/Memory/MemoryOperationsMeta.h>
#if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
#include <Stalker/Threading/ThreadOperations.h>
#endif
#if defined(STALKER_SIMD_AVX2_OK)
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX2.h>
#endif
#if defined(STALKER_SIMD_AVX512_OK)
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX512.h>
#endif

namespace Stalker::Memory {
    
    using namespace Stalker::Core;
    using namespace Stalker::Memory;
    #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
    using namespace Stalker::Threading;
    #endif

    class MemoryOperations {

    public:
    
        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void copy(size_t size, T* __restrict destination, const T* __restrict source){
            OperationExecutor<T_Operation::Copy, Trait, T>::call(size, destination, source);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void swap(size_t size, T* __restrict data1, T* __restrict data2){
            OperationExecutor<T_Operation::Swap, Trait, T>::call(size, data1, data2);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void setValue(size_t size, T* __restrict data, T value){
            OperationExecutor<T_Operation::SetValue, Trait, T>::call(size, data, value);
        }

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void copy(ThreadTrait& threadTrait, size_t n, T* destination, const T* source) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, OperationExecutor<T_Operation::Copy, Trait, T>(), n, destination, source);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void swap(ThreadTrait& threadTrait, size_t n, T* data1, T* data2) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::BinaryMutable>;
            Launcher::call(threadTrait, OperationExecutor<T_Operation::Swap, Trait, T>(), n, data1, data2);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void setValue(ThreadTrait& threadTrait, size_t size, T* data, T value) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Unary>;
            Launcher::call(threadTrait, OperationExecutor<T_Operation::SetValue, Trait, T>(), size, data, value);
        }

        #endif
        
        private:

        enum class T_Operation {
            Copy,
            Swap,
            SetValue,
        };

        template<typename Child, T_Operation OperationT, typename Trait, typename T>
        struct OperationExecutorBase {

            template<typename... Args>
            constexpr inline static auto call(Args&&... args) {
                Child::call(std::forward<Args>(args)...);
            }
        };

        template<T_Operation OperationT, typename Trait, typename T> struct OperationExecutor;

        template<typename Trait, typename T>
        struct OperationExecutor<T_Operation::Copy, Trait, T>
            : public OperationExecutorBase<OperationExecutor<T_Operation::Copy, Trait, T>, T_Operation::Copy, Trait, T> {

            template<typename... Args>
            constexpr inline static void call(Args&&... args) {
                
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    MemoryOperationsSIMD<T, Trait::SIMDArch>::template copy<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    MemoryOperationsMeta::copy<T, Trait::Unroll>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Scalar)
                    MemoryOperationsClassic::copy<T, Trait::IsSTD>(std::forward<Args>(args)...);
            }
        };

        template<typename Trait, typename T>
        struct OperationExecutor<T_Operation::Swap, Trait, T>
            : public OperationExecutorBase<OperationExecutor<T_Operation::Swap, Trait, T>, T_Operation::Swap, Trait, T> {

            template<typename... Args>
            constexpr inline static void call(Args&&... args) {
                
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    MemoryOperationsSIMD<T, Trait::SIMDArch>::template swap<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    MemoryOperationsMeta::swap<T, Trait::Unroll>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Scalar)
                    MemoryOperationsClassic::swap<T, Trait::IsSTD>(std::forward<Args>(args)...);
            }
        };

        template<typename Trait, typename T>
        struct OperationExecutor<T_Operation::SetValue, Trait, T>
            : public OperationExecutorBase<OperationExecutor<T_Operation::SetValue, Trait, T>, T_Operation::SetValue, Trait, T> {

            template<typename... Args>
            constexpr inline static void call(Args&&... args) {
                
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    MemoryOperationsSIMD<T, Trait::SIMDArch>::template setValue<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    MemoryOperationsMeta::setValue<T, Trait::Unroll>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Scalar)
                    MemoryOperationsClassic::setValue<T, Trait::IsSTD>(std::forward<Args>(args)...);
            }
        };
    };
}; // namespace Stalker::Memory