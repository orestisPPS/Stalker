#pragma once
#include <cstddef>
#include <Stalker/Core/Config/LoopUnrolling.h>
#include <Stalker/Core/Config/SIMD.h>

namespace Stalker::Core {

    using namespace Stalker::Core::Config;

    enum class ExecutionTraitType {
        Classic,
        UnrolledMeta,
        SIMD,
        None,
    };

    template <typename Child>
    struct ExecutionTraitBase {

    protected:
        inline Child& child() { return *static_cast<Child*>(this); }
        inline const Child& child() const { return *static_cast<const Child*>(this); }
    };


    template <typename Child>
    struct ExecutionTrait : public ExecutionTraitBase<Child> {
        static constexpr ExecutionTraitType Type = Child::Type;
    };

    struct ExecutionTraitClassic : public ExecutionTrait<ExecutionTraitClassic> {
        static constexpr ExecutionTraitType  Type = ExecutionTraitType::Classic;
    };

    template <size_t UnrollFactor = DefaultUnrollFactor()>
    struct ExecutionTraitUnrolledMeta : public ExecutionTrait<ExecutionTraitUnrolledMeta<UnrollFactor>>{
        static constexpr size_t              Unroll = UnrollFactor;
        static constexpr ExecutionTraitType  Type = ExecutionTraitType::UnrolledMeta;
    };

    template< SIMDType SIMDT = DefaultSIMDType(), std::size_t UnrollFactor = DefaultUnrollFactor(), SIMDStoreType  SIMDStoreT   = DefaultSIMDStoreType() >
    struct ExecutionTraitSIMD : public ExecutionTrait<ExecutionTraitSIMD<SIMDT, UnrollFactor,SIMDStoreT>> {
        static constexpr size_t             Unroll = UnrollFactor;
        static constexpr ExecutionTraitType Type   = ExecutionTraitType::SIMD;
        static constexpr SIMDType           SIMDArch        = SIMDT;  
        static constexpr SIMDStoreType      StorePolicy     = SIMDStoreT;
    };



    struct DefaultExecutionTrait {
    
        /*---------------- compile-time constants available to API -----*/
        inline static constexpr ExecutionTraitType Type   = IsSIMDEnabled()   ? ExecutionTraitType::SIMD :
                                                            IsUnrollEnabled() ? ExecutionTraitType::UnrolledMeta :
                                                                                ExecutionTraitType::Classic;
    
        static constexpr std::size_t Unroll =
            (Type == ExecutionTraitType::SIMD || Type == ExecutionTraitType::UnrolledMeta)
                ? Config::DefaultUnrollFactor()
                : 1;        // Classic path → no unrolling
    
        static constexpr SIMDType SIMDArch =
            (Type == ExecutionTraitType::SIMD)
                ? Config::DefaultSIMDType()
                : SIMDType::None;
    
        static constexpr SIMDStoreType StorePolicy =
            (Type == ExecutionTraitType::SIMD)
                ? Config::DefaultSIMDStoreType()
                : SIMDStoreType::Streamed;
    
        /*--- safeguard: catch mis-builds where SIMD is claimed but none set ---*/
        static_assert(!(Type == ExecutionTraitType::SIMD && SIMDArch == SIMDType::None),
                    "STALKER_SIMD_ENABLE is ON but no SIMD instruction set is active!");
    };

        

    






} // namespace Stalker::Core::ExecutionPolicies