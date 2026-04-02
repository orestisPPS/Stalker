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
#include <Stalker/Mathematics/Vector/VectorMathScalar.h>
#include <Stalker/Mathematics/Vector/VectorMathMeta.h>
#if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
#include <Stalker/Threading/ThreadOperations.h>
#endif
#if defined(STALKER_SIMD_AVX2_OK)
#include <Stalker/Mathematics/Vector/SIMD/AVX/VectorMathSIMDAVX2.h>
#endif
#if defined(STALKER_SIMD_AVX512_OK)
#include <Stalker/Mathematics/Vector/SIMD/AVX/VectorMathSIMDAVX512.h>
#endif

namespace Stalker::Mathematics {

    using namespace Stalker::Core;
    using namespace Stalker::Mathematics;
    #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0
    using namespace Stalker::Threading;
    #endif

    class VectorMath {
    public:
        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void add(size_t n, const T* a, const T* b, T* result){
            Dispatcher<T_Operation::Add, T, Trait>::call(n, a, b, result);
        }
        
        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void add(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            Dispatcher<T_Operation::Add, T, Trait>::call(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void axpy(size_t n, const T* a, const T* b, T* result, T scalar) {
            Dispatcher<T_Operation::Axpy, T, Trait>::call(n, a, b, result, scalar);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void subtract(size_t n, const T* a, const T* b, T* result){
            Dispatcher<T_Operation::Subtract, T, Trait>::call(n, a, b, result);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void subtract(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            Dispatcher<T_Operation::Subtract, T, Trait>::call(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void multiply(size_t n, const T* a, const T* b, T* result){
            Dispatcher<T_Operation::Multiply, T, Trait>::call(n, a, b, result);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void multiply(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            Dispatcher<T_Operation::Multiply, T, Trait>::call(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void scale(size_t n, const T* data, T* result, T scalar) {
            Dispatcher<T_Operation::Scale, T, Trait>::call(n, data, result, scalar);
        }
        
        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void scale(size_t n, T* data, T scalar) {
            Dispatcher<T_Operation::Scale, T, Trait>::call(n, data, scalar);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void addConstant(size_t n, const T* data, T* result, T constant) {
            Dispatcher<T_Operation::AddConstant, T, Trait>::call(n, data, result, constant);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void addConstant(size_t n, T* data, T constant) {
            Dispatcher<T_Operation::AddConstant, T, Trait>::call(n, data, constant);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static T sum(size_t n, const T* STALKER_RESTRICT data) {
            return Dispatcher<T_Operation::Sum, T, Trait>::call(n, data);
        }
        
        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static T sumOfSquares(size_t n, const T* STALKER_RESTRICT data) {
            return Dispatcher<T_Operation::SumOfSquares, T, Trait>::call(n, data);
        }
        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static T dot(size_t n, const T* STALKER_RESTRICT a, const T* STALKER_RESTRICT b) {
            return Dispatcher<T_Operation::DotProduct, T, Trait>::call(n, a, b);
        }

        template <typename T, typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static ResultT cross2D(size_t, const T* a, const T* b) {
            return VectorMathScalar::cross2D<T, ResultT>(a, b);
        }

        template <typename T, typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void cross3D(size_t, const T* a, const T* b, ResultT* result) {
            VectorMathScalar::cross3D<T, ResultT>(0, a, b, result);
        }

        template <typename T, typename Trait = ExecutionTraitScalar<>>
        STALKER_FORCE_INLINE constexpr static double normL1(size_t n, const T* STALKER_RESTRICT data) {
            static_assert(Trait::Type == T_ExecTrait::Scalar, "Operation normL1 is only implemented for scalar execution trait");
            return Norms::L1<T, Trait::IsSTD>(n, data);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static double normL2(size_t n, const T* STALKER_RESTRICT data) {
            return std::sqrt(Dispatcher<T_Operation::SumOfSquares, T, Trait>::call(n, data));
        }

        template <typename T, typename Trait = ExecutionTraitScalar<>>
        STALKER_FORCE_INLINE constexpr static double normLInf(size_t n, const T* STALKER_RESTRICT data) {
            static_assert(Trait::Type == T_ExecTrait::Scalar, "Operation normLInf is only implemented for scalar execution trait");
            return Norms::Linf<T, Trait::IsSTD>(n, data);
        }

        template <typename T, typename Trait = DefaultExecutionTrait, typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void normalize(size_t size, const T* STALKER_RESTRICT data, ResultT* STALKER_RESTRICT result) {
            static_assert(std::is_floating_point<ResultT>::value, "In-place normalization requires floating-point type result vector");
            double norm = normL2<T, Trait>(size, data);
            scale<T, Trait>(size, data, result, 1.0 / norm);
        }

        template <typename T, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void normalize(size_t size, T* STALKER_RESTRICT data) {
            static_assert(std::is_floating_point<T>::value, "In-place normalization requires floating-point type result vector");
            double norm = normL2<T, Trait>(size, data);
            scale<T, Trait>(size, data, 1.0 / norm);
        }

        template <typename T, typename Trait = ExecutionTraitScalar<>>
        STALKER_FORCE_INLINE constexpr static T max(size_t size, const T* STALKER_RESTRICT data) {
            static_assert(Trait::Type == T_ExecTrait::Scalar, "Operation max is only implemented for scalar execution trait");
            return VectorMathScalar::max<T, Trait::IsSTD>(size, data);
        }

        template <typename T, typename Trait = ExecutionTraitScalar<>>
        STALKER_FORCE_INLINE constexpr static size_t maxIndex(size_t size, const T* STALKER_RESTRICT data) {
            static_assert(Trait::Type == T_ExecTrait::Scalar, "Operation maxIndex is only implemented for scalar execution trait");
            return VectorMathScalar::maxIndex<T, Trait::IsSTD>(size, data);
        }

        template <typename T, typename Trait = ExecutionTraitScalar<>>
        STALKER_FORCE_INLINE constexpr static T min(size_t size, const T* STALKER_RESTRICT data) {
            static_assert(Trait::Type == T_ExecTrait::Scalar, "Operation min is only implemented for scalar execution trait");
            return VectorMathScalar::min<T, Trait::IsSTD>(size, data);
        }

        template <typename T, typename Trait = ExecutionTraitScalar<>>
        STALKER_FORCE_INLINE constexpr static size_t minIndex(size_t size, const T* STALKER_RESTRICT data) {
            static_assert(Trait::Type == T_ExecTrait::Scalar, "Operation minIndex is only implemented for scalar execution trait");
            return VectorMathScalar::minIndex<T, Trait::IsSTD>(size, data);
        }



        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static void add(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Add, T, Trait>(), n, a, b, result);
        }
        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static void add(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Add, T, Trait>(), n, a, b, result, scalarA, scalarB);
        }
        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static void axpy(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result, T scalar) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Axpy, T, Trait>(), n, a, b, result, scalar);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static void subtract(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Subtract, T, Trait>(), n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static void subtract(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Subtract, T, Trait>(), n, a, b, result);
        }
                
        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static void multiply(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Multiply, T, Trait>(), n, a, b, result);
        }
        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static void multiply(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Multiply, T, Trait>(), n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static void scale(ThreadTrait& threadTrait, size_t n, const T* data, T* result, T scalar) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Unary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Scale, T, Trait>(), n, data, result, scalar);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static void scale(ThreadTrait& threadTrait, size_t n, T* data, T scalar) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Unary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Scale, T, Trait>(), n, data, scalar);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static void addConstant(ThreadTrait& threadTrait, size_t n, const T* data, T* result, T constant) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Unary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::AddConstant, T, Trait>(), n, data, result, constant);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static void addConstant(ThreadTrait& threadTrait, size_t n, T* data, T constant) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Unary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::AddConstant, T, Trait>(), n, data, constant);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static T sum(ThreadTrait& threadTrait, size_t n, const T* STALKER_RESTRICT data) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::UnaryReduced>;
            return Launcher::call(threadTrait, Dispatcher<T_Operation::Sum, T, Trait>(), n, data);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static T sumOfSquares(ThreadTrait& threadTrait, size_t n, const T* STALKER_RESTRICT data) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::UnaryReduced>;
            return Launcher::call(threadTrait, Dispatcher<T_Operation::SumOfSquares, T, Trait>(), n, data);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static T dot(ThreadTrait& threadTrait, size_t n, const T* STALKER_RESTRICT a, const T* STALKER_RESTRICT b) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::BinaryReduced>;
            return Launcher::call(threadTrait, Dispatcher<T_Operation::DotProduct, T, Trait>(), n, a, b);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE static double normL2(ThreadTrait& threadTrait, size_t n, const T* STALKER_RESTRICT data) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::UnaryReduced>;
            auto sumOfSquares = Launcher::call(threadTrait, Dispatcher<T_Operation::SumOfSquares, T, Trait>(), n, data);
            return std::sqrt(sumOfSquares);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait, typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void normalize(ThreadTrait& threadTrait, size_t size, const T* STALKER_RESTRICT data, ResultT* STALKER_RESTRICT result) {
            static_assert(std::is_floating_point<ResultT>::value, "In-place normalization requires floating-point types");
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            double norm = normL2<T, ThreadTrait, Trait>(threadTrait, size, data);
            scale<T, ThreadTrait, Trait>(threadTrait, size, data, result, 1.0 / norm);
        }

        template <typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        STALKER_FORCE_INLINE constexpr static void normalize(ThreadTrait& threadTrait, size_t size, T* STALKER_RESTRICT data) {
            static_assert(std::is_floating_point<T>::value, "In-place normalization requires floating-point types");
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            double norm = normL2<T, ThreadTrait, Trait>(threadTrait, size, data);
            scale<T, ThreadTrait, Trait>(threadTrait, size, data, 1.0 / norm);
        }

        #endif

    private:

        enum class T_Operation {
            Add,
            Axpy,
            Subtract,
            Multiply,
            Divide,
            Scale,
            AddConstant,
            Sum,
            DotProduct,
            SumOfSquares
        };
        
        template<typename Child, T_Operation OperationT, typename T, typename Trait>
        struct DispatcherBase {
        template<typename... Args>
        STALKER_FORCE_INLINE constexpr static auto call(Args&&... args) {
            using Ret = decltype(Child::call(std::forward<Args>(args)...));
            if constexpr ( std::is_void_v<Ret> )
                Child::call(std::forward<Args>(args)...);
            else
                return Child::call(std::forward<Args>(args)...);
            }
        };

        template<T_Operation OperationT, typename T, typename Trait> struct Dispatcher;

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::Add, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::Add, T, Trait>, T_Operation::Add, T, Trait> {
            template<typename... Args>
            STALKER_FORCE_INLINE constexpr static void call(Args&&... args) {
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    VectorMathSIMD<T, Trait::SIMDArch>::template add<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    VectorMathMeta::add<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    VectorMathScalar::add<T>(std::forward<Args>(args)...);    
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::Axpy, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::Axpy, T, Trait>, T_Operation::Axpy, T, Trait> {
            template<typename... Args>
            STALKER_FORCE_INLINE constexpr static void call(Args&&... args) {
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    VectorMathSIMD<T, Trait::SIMDArch>::template axpy<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    VectorMathMeta::axpy<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    VectorMathScalar::axpy<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::Subtract, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::Subtract, T, Trait>, T_Operation::Subtract, T, Trait> {
            template<typename... Args>
            STALKER_FORCE_INLINE constexpr static void call(Args&&... args) { 
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    VectorMathSIMD<T, Trait::SIMDArch>::template subtract<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    VectorMathMeta::subtract<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    VectorMathScalar::subtract<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::Multiply, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::Multiply, T, Trait>, T_Operation::Multiply, T, Trait> {
            template<typename... Args>
            STALKER_FORCE_INLINE constexpr static void call(Args&&... args) {

                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    VectorMathSIMD<T, Trait::SIMDArch>::template multiply<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    VectorMathMeta::multiply<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    VectorMathScalar::multiply<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::Scale, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::Scale, T, Trait>, T_Operation::Scale, T, Trait> {
            template<typename... Args>
            STALKER_FORCE_INLINE constexpr static void call(Args&&... args) {
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    VectorMathSIMD<T, Trait::SIMDArch>::template scale<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    VectorMathMeta::scale<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    VectorMathScalar::scale<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::AddConstant, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::AddConstant, T, Trait>, T_Operation::AddConstant, T, Trait> {
            template<typename... Args>
            STALKER_FORCE_INLINE constexpr static void call(Args&&... args) {                
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    VectorMathSIMD<T, Trait::SIMDArch>::template addConstant<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    VectorMathMeta::addConstant<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    VectorMathScalar::addConstant<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::Sum, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::Sum, T, Trait>, T_Operation::Sum, T, Trait> {
            template<typename... Args>
            STALKER_FORCE_INLINE constexpr static auto call(Args&&... args) {             
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    return VectorMathSIMD<T, Trait::SIMDArch>::template sum<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    return VectorMathMeta::sum<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    return VectorMathScalar::sum<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::SumOfSquares, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::SumOfSquares, T, Trait>, T_Operation::SumOfSquares, T, Trait> {
            template<typename... Args>
            STALKER_FORCE_INLINE constexpr static auto call(Args&&... args) {             
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    return VectorMathSIMD<T, Trait::SIMDArch>::template sumOfSquares<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    return VectorMathMeta::sumOfSquares<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    return VectorMathScalar::sumOfSquares<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::DotProduct, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::DotProduct, T, Trait>, T_Operation::DotProduct, T, Trait> {
            template<typename... Args>
            STALKER_FORCE_INLINE constexpr static auto call(Args&&... args) {             
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    return VectorMathSIMD<T, Trait::SIMDArch>::template dot<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    return VectorMathMeta::dot<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    return VectorMathScalar::dot<T>(std::forward<Args>(args)...);
            }
        };
    };
        
};// namespace Stalker::Mathematics::Operations