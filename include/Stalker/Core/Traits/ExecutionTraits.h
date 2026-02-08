#pragma once
#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDBase.h>

namespace Stalker::Core {

    using namespace Stalker::Core::Config;

    enum class T_ExecTrait {
        Scalar,
        Unrolled,
        SIMD,
        None,
    };


    template <typename Child>
    struct ExecutionTrait {
        static constexpr T_ExecTrait Type = Child::_Type();

        template <typename T>
        static constexpr size_t BlockSize(){
            return Child::template _BlockSize<T>();
        }

    protected:
        inline Child& child() { return *static_cast<Child*>(this); }
        inline const Child& child() const { return *static_cast<const Child*>(this); }
    };

    template <bool EnableSTD = true>
    struct ExecutionTraitScalar : public ExecutionTrait<ExecutionTraitScalar<EnableSTD>> {
        static constexpr bool IsSTD = EnableSTD;

    protected:
        friend ExecutionTrait<ExecutionTraitScalar<EnableSTD>>;

        static constexpr T_ExecTrait _Type() {
            return T_ExecTrait::Scalar;
        }

        template <typename T>
        static constexpr size_t _BlockSize(){
            return 1;
        }
    };

    template <size_t UnrollFactor = DefaultUnroll()>
    struct ExecutionTraitUnrolled : public ExecutionTrait<ExecutionTraitUnrolled<UnrollFactor>>{
        static constexpr size_t Unroll = UnrollFactor;
    protected:
        friend ExecutionTrait<ExecutionTraitUnrolled<UnrollFactor>>;

        static constexpr T_ExecTrait _Type() {
            return T_ExecTrait::Unrolled;
        }

        template <typename T>
        static constexpr size_t _BlockSize(){
            return Unroll;
        }
    };

    template<T_SIMD          SIMDT            = DefaultSIMDType(),
             bool            Aligned          = false,
             T_SIMDStore     SIMDStoreT       = DefaultSIMDStore(),
             size_t          UnrollFactor     = DefaultUnroll(),
             T_PrefetchHints PrefetchH        = DefaultPrefetchHint()>
    struct ExecutionTraitSIMD : public ExecutionTrait<ExecutionTraitSIMD<SIMDT, Aligned, SIMDStoreT, UnrollFactor, PrefetchH>> {
        static constexpr T_SIMD          SIMDArch         = SIMDT;  
        static constexpr bool            IsAligned        = Aligned;
        static constexpr T_SIMDStore     StorePolicy      = SIMDStoreT;
        static constexpr size_t          Unroll           = UnrollFactor;
        static constexpr T_PrefetchHints PrefetchHint     = PrefetchH;

    protected:

        friend ExecutionTrait<ExecutionTraitSIMD<SIMDT, Aligned, SIMDStoreT, UnrollFactor, PrefetchH>>;
        static constexpr T_ExecTrait _Type() {
            return T_ExecTrait::SIMD;
        }
        
        template <typename T>
        static constexpr size_t _BlockSize(){
            return TypeTraitsSIMD<T, SIMDArch>::RegisterSize() * Unroll;
        }
    };



    struct DefaultExecutionTrait {
    
        /*---------------- compile-time constants available to API -----*/
        inline static constexpr T_ExecTrait Type = IsSIMDOk() ? T_ExecTrait::SIMD : DefaultUnroll() > 1 ? T_ExecTrait::Unrolled : T_ExecTrait::Scalar;

        static constexpr T_SIMD SIMDArch = (Type == T_ExecTrait::SIMD) ? Config::DefaultSIMDType() : T_SIMD::None;
        
        static constexpr bool IsAligned = false;

        static constexpr T_SIMDStore StorePolicy = (Type == T_ExecTrait::SIMD) ? Config::DefaultSIMDStore() : T_SIMDStore::Streamed;
        
        static constexpr size_t Unroll = (Type == T_ExecTrait::Unrolled || Type == T_ExecTrait::SIMD) ? Config::DefaultUnroll() : 1;
        
        static constexpr T_PrefetchHints PrefetchHint =  (Type == T_ExecTrait::SIMD) ? Config::DefaultPrefetchHint() : T_PrefetchHints::HintNone;
        
        static constexpr bool UseSTD = true;

    
        /*--- safeguard: catch mis-builds where SIMD is claimed but none set ---*/
        static_assert(!(Type == T_ExecTrait::SIMD && SIMDArch == T_SIMD::None),
                    "STALKER_SIMD_ENABLE is ON but no SIMD instruction set is active!");
    };

        

    






} // namespace Stalker::Core::ExecutionPolicies