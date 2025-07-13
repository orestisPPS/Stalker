#pragma once
#include <Stalker/Core/Traits/ExecutionTraits.h>
#include <Stalker/Core/Traits/ThreadingTraits.h>
#include <Stalker/Mathematics/Operations/MathOperationsClassic.h>
#include <Stalker/Mathematics/Operations/MathOperationsMeta.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDAVX2.h>
#include <Stalker/Mathematics/Operations/SIMD/MathOperationsSIMDAVX512.h>


namespace Stalker::Mathematics {

    using namespace Stalker::Core;
    using namespace Stalker::Mathematics;
    using namespace Stalker::Mathematics::SIMD;

    class MathOperations {
    public:
        template<typename T, typename ExecTrait = DefaultExecutionTrait>
        constexpr inline static void add(size_t n, const T* a, const T* b, T* result){
            OperationExecutor<OperationType::Add, ExecTrait, T>::call(n, a, b, result);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void add(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            OperationExecutor<OperationType::Add, Exec, T>::call(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename ExecTrait = DefaultExecutionTrait>
        constexpr inline static void subtract(size_t n, const T* a, const T* b, T* result){
            OperationExecutor<OperationType::Subtract, ExecTrait, T>::call(n, a, b, result);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void subtract(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            OperationExecutor<OperationType::Subtract, Exec, T>::call(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename ExecTrait = DefaultExecutionTrait>
        constexpr inline static void multiply(size_t n, const T* a, const T* b, T* result){
            OperationExecutor<OperationType::Multiply, ExecTrait, T>::call(n, a, b, result);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void multiply(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            OperationExecutor<OperationType::Multiply, Exec, T>::call(n, a, b, result, scalarA, scalarB);
        }

    private:

        // struct AddOp {
        //     template<class ExecType, typename ...FArgs, auto ...TArgs>
        //     static constexpr void call(FArgs&&... args) {
        //         ExecType::template add<TArgs...>(std::forward<FArgs>(args)...);
        //     }
        // };
        //     template <typename ExecType, typename... TArgs, typename... FArgs>
        //     constexpr inline static void call(FArgs&&... args) {
        //         ExecType::template addConstant<TArgs...>(std::forward<FArgs>(args)...);
        //     }
        // };

        enum class OperationType {
            Add,
            Subtract,
            Multiply,
            Divide,
            Scale,
            AddConstant
        };
        
        // template<typename ExecTrait>
        // constexpr inline static bool IsSIMDEnabled() {
        //     return ExecTrait::Type == ExecutionTraitType::SIMD && Stalker::Core::IsSIMDEnabled();
        // }

        // template<typename ExecTrait>
        // constexpr inline static bool IsUnrollEnabled() {
        //     return ExecTrait::Type == ExecutionTraitType::UnrolledMeta && Stalker::Core::IsUnrollEnabled();
        // }

        template<typename Child, OperationType OperationT, typename ExecTrait, typename T>
        struct OperationExecutorBase {
        template<typename... Args>
        constexpr inline static auto call(Args&&... args) {
            using Ret = decltype(Child::call(std::forward<Args>(args)...));
            if constexpr ( std::is_void_v<Ret> )
                Child::call(std::forward<Args>(args)...);
            else
                return Child::call(std::forward<Args>(args)...);
            }
        };

        template<OperationType OperationT, typename ExecTrait, typename T> struct OperationExecutor;

        template<typename ExecTrait, typename T>
        struct OperationExecutor<OperationType::Add, ExecTrait, T>
            : public OperationExecutorBase<OperationExecutor<OperationType::Add, ExecTrait, T>, OperationType::Add, ExecTrait, T> {
            template<typename... Args>
            constexpr inline static void call(Args&&... args) {
                constexpr auto execTrait = ExecTrait::Type;
                if constexpr (execTrait == ExecutionTraitType::SIMD && IsSIMDEnabled())
                    MathOperationsSIMD<T, ExecTrait::SIMDArch>::template add<ExecTrait::Unroll, ExecTrait::StorePolicy>(std::forward<Args>(args)...);
                else if constexpr (execTrait == ExecutionTraitType::UnrolledMeta && IsUnrollEnabled())
                    MathOperationsMeta::add<T, ExecTrait::Unroll>(std::forward<Args>(args)...);
                else
                    MathOperationsClassic::add<T>(std::forward<Args>(args)...);    
            }
        };

        template<typename ExecTrait, typename T>
        struct OperationExecutor<OperationType::Subtract, ExecTrait, T>
            : public OperationExecutorBase<OperationExecutor<OperationType::Subtract, ExecTrait, T>, OperationType::Subtract, ExecTrait, T> {
            template<typename... Args>
            constexpr inline static void call(Args&&... args) { 
                constexpr auto execTrait = ExecTrait::Type;
                if constexpr (execTrait == ExecutionTraitType::SIMD && IsSIMDEnabled())
                    MathOperationsSIMD<T, ExecTrait::SIMDArch>::template subtract<ExecTrait::Unroll, ExecTrait::StorePolicy>(std::forward<Args>(args)...);
                else if constexpr (execTrait == ExecutionTraitType::UnrolledMeta && IsUnrollEnabled())
                    MathOperationsMeta::subtract<T, ExecTrait::Unroll>(std::forward<Args>(args)...);
                else
                    MathOperationsClassic::subtract<T>(std::forward<Args>(args)...);
            }
        };

        template<typename ExecTrait, typename T>
        struct OperationExecutor<OperationType::Multiply, ExecTrait, T>
            : public OperationExecutorBase<OperationExecutor<OperationType::Multiply, ExecTrait, T>, OperationType::Multiply, ExecTrait, T> {
            template<typename... Args>
            constexpr inline static void call(Args&&... args) {
                constexpr auto execTrait = ExecTrait::Type;
                if constexpr (execTrait == ExecutionTraitType::SIMD && IsSIMDEnabled())
                    MathOperationsSIMD<T, ExecTrait::SIMDArch>::template multiply<ExecTrait::Unroll, ExecTrait::StorePolicy>(std::forward<Args>(args)...);
                else if constexpr (execTrait == ExecutionTraitType::UnrolledMeta && IsUnrollEnabled())
                    MathOperationsMeta::multiply<T, ExecTrait::Unroll>(std::forward<Args>(args)...);
                else
                    MathOperationsClassic::multiply<T>(std::forward<Args>(args)...);
            }
        };

    };
        
};// namespace Stalker::Mathematics::Operations