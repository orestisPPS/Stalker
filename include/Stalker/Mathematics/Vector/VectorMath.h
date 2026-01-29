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
        constexpr inline static void add(size_t n, const T* a, const T* b, T* result){
            Dispatcher<T_Operation::Add, T, Trait>::call(n, a, b, result);
        }
        
        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void add(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            Dispatcher<T_Operation::Add, T, Trait>::call(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void axpy(size_t n, const T* a, const T* b, T* result, T scalar) {
            Dispatcher<T_Operation::Axpy, T, Trait>::call(n, a, b, result, scalar);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void subtract(size_t n, const T* a, const T* b, T* result){
            Dispatcher<T_Operation::Subtract, T, Trait>::call(n, a, b, result);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void subtract(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            Dispatcher<T_Operation::Subtract, T, Trait>::call(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void multiply(size_t n, const T* a, const T* b, T* result){
            Dispatcher<T_Operation::Multiply, T, Trait>::call(n, a, b, result);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void multiply(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            Dispatcher<T_Operation::Multiply, T, Trait>::call(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void scale(size_t n, const T* data, T* result, T scalar) {
            Dispatcher<T_Operation::Scale, T, Trait>::call(n, data, result, scalar);
        }
        
        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void scale(size_t n, T* data, T scalar) {
            Dispatcher<T_Operation::Scale, T, Trait>::call(n, data, scalar);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void addConstant(size_t n, const T* data, T* result, T constant) {
            Dispatcher<T_Operation::AddConstant, T, Trait>::call(n, data, result, constant);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void addConstant(size_t n, T* data, T constant) {
            Dispatcher<T_Operation::AddConstant, T, Trait>::call(n, data, constant);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static T sum(size_t n, const T* __restrict data) {
            return Dispatcher<T_Operation::Sum, T, Trait>::call(n, data);
        }
        
        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static T dot(size_t n, const T* __restrict a, const T* __restrict b) {
            return Dispatcher<T_Operation::DotProduct, T, Trait>::call(n, a, b);
        }

        #if defined(STALKER_THREADING_ENABLE) && STALKER_THREADING_ENABLE != 0

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void add(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Add, T, Trait>(), n, a, b, result);
        }
        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void add(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Add, T, Trait>(), n, a, b, result, scalarA, scalarB);
        }
        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void axpy(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result, T scalar) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Axpy, T, Trait>(), n, a, b, result, scalar);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void subtract(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Subtract, T, Trait>(), n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void subtract(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Subtract, T, Trait>(), n, a, b, result);
        }
                
        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void multiply(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Multiply, T, Trait>(), n, a, b, result);
        }
        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void multiply(ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Multiply, T, Trait>(), n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void scale(ThreadTrait& threadTrait, size_t n, const T* data, T* result, T scalar) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Unary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Scale, T, Trait>(), n, data, result, scalar);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void scale(ThreadTrait& threadTrait, size_t n, T* data, T scalar) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Unary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Scale, T, Trait>(), n, data, scalar);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void addConstant(ThreadTrait& threadTrait, size_t n, const T* data, T* result, T constant) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Unary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::AddConstant, T, Trait>(), n, data, result, constant);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void addConstant(ThreadTrait& threadTrait, size_t n, T* data, T constant) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Unary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::AddConstant, T, Trait>(), n, data, constant);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static T sum(ThreadTrait& threadTrait, size_t n, const T* __restrict data) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::UnaryReduced>;
            return Launcher::call(threadTrait, Dispatcher<T_Operation::Sum, T, Trait>(), n, data);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static T dot(ThreadTrait& threadTrait, size_t n, const T* __restrict a, const T* __restrict b) {
            threadTrait.setLoopBlockSize(Trait::template BlockSize<T>());
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::BinaryReduced>;
            return Launcher::call(threadTrait, Dispatcher<T_Operation::DotProduct, T, Trait>(), n, a, b);
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
            DotProduct
        };
        
        template<typename Child, T_Operation OperationT, typename T, typename Trait>
        struct DispatcherBase {
        template<typename... Args>
        constexpr inline static auto call(Args&&... args) {
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
            constexpr inline static void call(Args&&... args) {
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
            constexpr inline static void call(Args&&... args) {
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
            constexpr inline static void call(Args&&... args) { 
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
            constexpr inline static void call(Args&&... args) {

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
            constexpr inline static void call(Args&&... args) {
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
            constexpr inline static void call(Args&&... args) {                
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
            constexpr inline static auto call(Args&&... args) {             
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDOk())
                    return VectorMathSIMD<T, Trait::SIMDArch>::template sum<Trait>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    return VectorMathMeta::sum<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    return VectorMathScalar::sum<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::DotProduct, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::DotProduct, T, Trait>, T_Operation::DotProduct, T, Trait> {
            template<typename... Args>
            constexpr inline static auto call(Args&&... args) {             
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