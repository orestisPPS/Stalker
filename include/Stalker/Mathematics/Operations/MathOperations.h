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
                template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void add(size_t n, const T* a, const T* b, T* result){
            _execute<T, Exec, AddOp>(n, a, b, result);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void add(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            _execute<T, Exec, AddOp>(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void subtract(size_t n, const T* a, const T* b, T* result) {
            _execute<T, Exec, SubtractOp>(n, a, b, result);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void subtract(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            _execute<T, Exec, SubtractOp>(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void multiply(size_t n, const T* a, const T* b, T* result) {
            _execute<T, Exec, MultiplyOp>(n, a, b, result);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void multiply(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            _execute<T, Exec, MultiplyOp>(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void divide(size_t n, const T* a, const T* b, T* result) {
            _execute<T, Exec, DivideOp>(n, a, b, result);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void divide(size_t n, const T* a, const T* b, T* result, T scalarA, T scalarB) {
            _execute<T, Exec, DivideOp>(n, a, b, result, scalarA, scalarB);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void scale(size_t n, const T* data, T* result, T scalar) {
            _execute<T, Exec, ScaleOp>(n, data, result, scalar);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void scale(size_t n, T* data, T scalar) {
            _execute<T, Exec, ScaleOp>(n, data, scalar);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void addConstant(size_t n, const T* data, T* result, T constant) {
            _execute<T, Exec, AddConstantOp>(n, data, result, constant);
        }

        template<typename T, typename Exec = DefaultExecutionTrait>
        constexpr inline static void addConstant(size_t n, const T* data, T constant) {
            _execute<T, Exec, AddConstantOp>(n, data, constant);
        }

    private:

        template<typename T, typename ExecTrait, typename Operation, typename... Args>
        constexpr inline static void _execute(Args&&... args) {
            constexpr auto ExecT = ExecTrait::Type;
            constexpr auto Unroll = ExecTrait::Unroll;
            constexpr auto StoreT = ExecTrait::StorePolicy;
            
            if constexpr (ExecT == ExecutionTraitType::SIMD && IsSIMDEnabled()) {
                Operation::template call<MathOperationsSIMD<T, ExecTrait::SIMDArch>, Unroll, StoreT>(std::forward<Args>(args)...);
            } 
            else if constexpr (ExecT == ExecutionTraitType::UnrolledMeta && IsUnrollEnabled()) {
                Operation::template call<MathOperationsMeta, T, Unroll>(std::forward<Args>(args)...);
            } 
            else {
                Operation::template call<MathOperationsClassic, T>(std::forward<Args>(args)...);
            }
        }

        struct AddOp {
            template <typename ExecType, typename... Args>
            constexpr inline static void call(Args&&... args) {
                ExecType::template add(std::forward<Args>(args)...);
            }
        };

        struct SubtractOp {
            template <typename ExecType, typename... Args>
            constexpr inline static void call(Args&&... args) {
                ExecType::template subtract(std::forward<Args>(args)...);
            }
        };

        struct MultiplyOp {
            template <typename ExecType, typename... Args>
            constexpr inline static void call(Args&&... args) {
                ExecType::template multiply(std::forward<Args>(args)...);
            }
        };
        struct DivideOp {
            template <typename ExecType, typename... Args>
            constexpr inline static void call(Args&&... args) {
                ExecType::template divide(std::forward<Args>(args)...);
            }
        };  

        struct ScaleOp {
            template <typename ExecType, typename... Args>
            constexpr inline static void call(Args&&... args) {
                ExecType::template scale(std::forward<Args>(args)...);
            }
        };  

        struct AddConstantOp {
            template <typename ExecType, typename... Args>
            constexpr inline static void call(Args&&... args) {
                ExecType::template addConstant(std::forward<Args>(args)...);
            }
        };
    };
};// namespace Stalker::Mathematics::Operations