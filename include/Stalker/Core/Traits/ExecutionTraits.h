#pragma once
#include <Stalker/Core/Config/LoopUnrolling.h>
#include <Stalker/Core/Config/SIMD.h>

namespace Stalker::Core {

    using namespace Stalker::Core::Config;

    enum class T_ExecTrait {
        Classic,
        Unrolled,
        SIMD,
        None,
    };


    template <typename Child>
    struct ExecutionTrait {
        static constexpr T_ExecTrait Type = Child::Type;

    protected:
        inline Child& child() { return *static_cast<Child*>(this); }
        inline const Child& child() const { return *static_cast<const Child*>(this); }
    };

    template <bool EnableSTD = true>
    struct ExecutionTraitClassic : public ExecutionTrait<ExecutionTraitClassic<EnableSTD>> {
        static constexpr T_ExecTrait Type   = T_ExecTrait::Classic;
        /// Use std algorithm if applicable
        static constexpr bool               IsSTD = EnableSTD;
    };

    template <size_t UnrollFactor = DefaultUnroll()>
    struct ExecutionTraitUnrolled : public ExecutionTrait<ExecutionTraitUnrolled<UnrollFactor>>{
        static constexpr size_t              Unroll = UnrollFactor;
        static constexpr T_ExecTrait  Type   = T_ExecTrait::Unrolled;
    };

    template< T_SIMD SIMDT = DefaultSIMDType(),
              std::size_t UnrollFactor  = DefaultUnroll(),
              T_SIMDStore  SIMDStoreT = DefaultSIMDStore() >
    struct ExecutionTraitSIMD : public ExecutionTrait<ExecutionTraitSIMD<SIMDT, UnrollFactor, SIMDStoreT>> {
        static constexpr size_t             Unroll      = UnrollFactor;
        static constexpr T_ExecTrait Type        = T_ExecTrait::SIMD;
        static constexpr T_SIMD           SIMDArch    = SIMDT;  
        static constexpr T_SIMDStore    StorePolicy = SIMDStoreT;
    };



    struct DefaultExecutionTrait {
    
        /*---------------- compile-time constants available to API -----*/
        inline static constexpr T_ExecTrait Type   = IsSIMDEnabled()     ? T_ExecTrait::SIMD :
                                                            DefaultUnroll() > 1 ? T_ExecTrait::Unrolled :
                                                            T_ExecTrait::Classic;
    
        static constexpr std::size_t Unroll =
            (Type == T_ExecTrait::SIMD || Type == T_ExecTrait::Unrolled)
                ? Config::DefaultUnroll()
                : 1;        // Classic path → no unrolling
    
        static constexpr T_SIMD SIMDArch =
            (Type == T_ExecTrait::SIMD)
                ? Config::DefaultSIMDType()
                : T_SIMD::None;
    
        static constexpr T_SIMDStore StorePolicy =
            (Type == T_ExecTrait::SIMD)
                ? Config::DefaultSIMDStore()
                : T_SIMDStore::Streamed;

        static constexpr bool UseSTD = true;
    
        /*--- safeguard: catch mis-builds where SIMD is claimed but none set ---*/
        static_assert(!(Type == T_ExecTrait::SIMD && SIMDArch == T_SIMD::None),
                    "STALKER_SIMD_ENABLE is ON but no SIMD instruction set is active!");
    };

        

    






} // namespace Stalker::Core::ExecutionPolicies