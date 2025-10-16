#pragma once
#include <Stalker/Core/Traits/ExecutionTraits.h>
#include <Stalker/Threading/ThreadOperations.h>
#include <Stalker/Mathematics/Operations/MathOperationsClassic.h>
#include <Stalker/Mathematics/Operations/MathOperationsMeta.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDAVX2.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDAVX512.h>


namespace Stalker::Mathematics {

    using namespace Stalker::Core;
    using namespace Stalker::Mathematics;
    using namespace Stalker::Mathematics::SIMD;
    using namespace Stalker::Threading;

    class MathOperations {
    public:
        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void add(size_t n, const T* a, const T* b, T* result){
            Dispatcher<T_Operation::Add, T, Trait>::call(n, a, b, result);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void add(const ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result) {
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Add, T, Trait>(), n, a, b, result);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void add(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            Dispatcher<T_Operation::Add, T, Trait>::call(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void add(const ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Add, T, Trait>(), n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void subtract(size_t n, const T* a, const T* b, T* result){
            Dispatcher<T_Operation::Subtract, T, Trait>::call(n, a, b, result);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void subtract(const ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result) {
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Subtract, T, Trait>(), n, a, b, result);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void subtract(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            Dispatcher<T_Operation::Subtract, T, Trait>::call(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void subtract(const ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Subtract, T, Trait>(), n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void multiply(size_t n, const T* a, const T* b, T* result){
            Dispatcher<T_Operation::Multiply, T, Trait>::call(n, a, b, result);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void multiply(const ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result) {
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Multiply, T, Trait>(), n, a, b, result);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void multiply(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            Dispatcher<T_Operation::Multiply, T, Trait>::call(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void multiply(const ThreadTrait& threadTrait, size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Binary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Multiply, T, Trait>(), n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void scale(size_t n, const T* data, T* result, T scalar) {
            Dispatcher<T_Operation::Scale, T, Trait>::call(n, data, result, scalar);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void scale(const ThreadTrait& threadTrait, size_t n, const T* data, T* result, T scalar) {
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Unary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::Scale, T, Trait>(), n, data, result, scalar);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static void addConstant(size_t n, const T* data, T* result, T constant) {
            Dispatcher<T_Operation::AddConstant, T, Trait>::call(n, data, result, constant);
        }

        template<typename T, typename ThreadTrait, typename Trait = DefaultExecutionTrait>
        constexpr inline static void addConstant(const ThreadTrait& threadTrait, size_t n, const T* data, T* result, T constant) {
            using Launcher = Launcher<T, ThreadTrait, T_ThreadOperation::Unary>;
            Launcher::call(threadTrait, Dispatcher<T_Operation::AddConstant, T, Trait>(), n, data, result, constant);
        }

        template<typename T, typename Trait = DefaultExecutionTrait>
        constexpr inline static T sum(size_t n, const T* __restrict data) {
            return Dispatcher<T_Operation::Sum, T, Trait>::call(n, data);
        }

    private:

        enum class T_Operation {
            Add,
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
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDEnabled())
                    MathOperationsSIMD<T, Trait::SIMDArch>::template add<Trait::Unroll, Trait::StorePolicy>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    MathOperationsMeta::add<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    MathOperationsClassic::add<T>(std::forward<Args>(args)...);    
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::Subtract, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::Subtract, T, Trait>, T_Operation::Subtract, T, Trait> {
            template<typename... Args>
            constexpr inline static void call(Args&&... args) { 
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDEnabled())
                    MathOperationsSIMD<T, Trait::SIMDArch>::template subtract<Trait::Unroll, Trait::StorePolicy>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    MathOperationsMeta::subtract<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    MathOperationsClassic::subtract<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::Multiply, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::Multiply, T, Trait>, T_Operation::Multiply, T, Trait> {
            template<typename... Args>
            constexpr inline static void call(Args&&... args) {

                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDEnabled())
                    MathOperationsSIMD<T, Trait::SIMDArch>::template multiply<Trait::Unroll, Trait::StorePolicy>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    MathOperationsMeta::multiply<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    MathOperationsClassic::multiply<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::Scale, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::Scale, T, Trait>, T_Operation::Scale, T, Trait> {
            template<typename... Args>
            constexpr inline static void call(Args&&... args) {
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDEnabled())
                    MathOperationsSIMD<T, Trait::SIMDArch>::template scale<Trait::Unroll, Trait::StorePolicy>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    MathOperationsMeta::scale<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    MathOperationsClassic::scale<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::AddConstant, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::AddConstant, T, Trait>, T_Operation::AddConstant, T, Trait> {
            template<typename... Args>
            constexpr inline static void call(Args&&... args) {                
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDEnabled())
                    MathOperationsSIMD<T, Trait::SIMDArch>::template addConstant<Trait::Unroll, Trait::StorePolicy>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    MathOperationsMeta::addConstant<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    MathOperationsClassic::addConstant<T>(std::forward<Args>(args)...);
            }
        };

        template<typename T, typename Trait>
        struct Dispatcher<T_Operation::Sum, T, Trait>
            : public DispatcherBase<Dispatcher<T_Operation::Sum, T, Trait>, T_Operation::Sum, T, Trait> {
            template<typename... Args>
            constexpr inline static auto call(Args&&... args) {             
                if constexpr (Trait::Type == T_ExecTrait::SIMD && IsSIMDEnabled())
                    return MathOperationsSIMD<T, Trait::SIMDArch>::template sum<Trait::Unroll>(std::forward<Args>(args)...);
                else if constexpr (Trait::Type == T_ExecTrait::Unrolled)
                    return MathOperationsMeta::sum<T, Trait::Unroll>(std::forward<Args>(args)...);
                else
                    return MathOperationsClassic::sum<T>(std::forward<Args>(args)...);
            }
        };

    };
        
};// namespace Stalker::Mathematics::Operations