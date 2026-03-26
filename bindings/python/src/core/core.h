#pragma once
#include <nanobind/nanobind.h>
#include <Stalker/Core/Traits/ExecutionTraits.h>

using namespace Stalker::Core;
using namespace Stalker::Core::Config;

/**
 * @brief Alignment dispatch policy for SIMD execution kernels.
 *
 * Mirrors the compile-time Aligned template parameter of ExecutionTraitSIMD<>
 * from ExecutionTraits.h, elevated to a runtime-selectable policy:
 *   Auto      – Inspect pointer addresses at dispatch time; choose optimal path.
 *   Aligned   – Assert all buffers satisfy SIMD alignment; enables aligned loads/stores.
 *   Unaligned – Force unaligned intrinsic paths regardless of actual alignment.
 */
enum class AlignmentPolicy {
    Auto      = -1,
    Aligned   =  1,
    Unaligned =  0
};

/**
 * @brief Dynamic runtime proxy for statically compiled HPC Execution Traits.
 * 
 * In C++, high-performance compute logic demands compile-time instruction 
 * routing (SIMD vs. Unrolled vs. Scalar) to maximize compiler instruction 
 * parallelism. This lightweight class acts as an opaque handle, accepting 
 * runtime Python properties which are statically mapped via recursive template
 * unwrapping (see `dispatch.h`) to native compile-time configurations.
 */
struct ExecutionTraitPy {

    /// @brief Primary architectural backend trait (Scalar, Unrolled, SIMD, Default)
    T_ExecTrait type = T_ExecTrait::None; 
    
    bool enable_std = false;
    size_t unroll_factor = 1;

    // SIMD configuration overrides
    T_SIMD          simd_type      = T_SIMD::None;
    T_SIMDStore     store_policy   = T_SIMDStore::Cached;
    T_PrefetchHints prefetch_hint  = T_PrefetchHints::HintNone;
    T_ILPPolicy     ilp_policy       = T_ILPPolicy::Interleaved;
    AlignmentPolicy alignment_policy = AlignmentPolicy::Auto;
    
    /// @brief Fallback to the generalized implicit architecture definitions.
    static ExecutionTraitPy Default() {
        return {T_ExecTrait::None, true, 1, T_SIMD::None, T_SIMDStore::Cached, T_PrefetchHints::HintNone, T_ILPPolicy::Interleaved, AlignmentPolicy::Auto};
    }
    
    /// @brief Route through the classical non-vectorized path.
    static ExecutionTraitPy Scalar(bool enable_std = true) {
        return {T_ExecTrait::Scalar, enable_std, 1, T_SIMD::None, T_SIMDStore::Cached, T_PrefetchHints::HintNone, T_ILPPolicy::Interleaved, AlignmentPolicy::Auto};
    }
    
    /// @brief Compel static loop unrolling optimization strategies.
    static ExecutionTraitPy Unrolled(size_t unroll = 1) {
        return {T_ExecTrait::Unrolled, false, unroll, T_SIMD::None, T_SIMDStore::Cached, T_PrefetchHints::HintNone, T_ILPPolicy::Interleaved, AlignmentPolicy::Auto};
    }
    
    /// @brief Expose raw vector instruction tuning for explicit performance tuning.
    static ExecutionTraitPy SIMD(
        T_SIMD simd_type = Config::T_SIMD::AVX2,
        size_t unroll = 1, 
        T_SIMDStore store = Config::T_SIMDStore::Cached, 
        T_PrefetchHints prefetch = Config::T_PrefetchHints::HintNone,
        T_ILPPolicy ilp = Config::T_ILPPolicy::Interleaved,
        AlignmentPolicy aligned = AlignmentPolicy::Auto
    ) {
        return {T_ExecTrait::SIMD, false, unroll, simd_type, store, prefetch, ilp, aligned};
    }
};
void bind_core(nanobind::module_& m);
