#pragma once
#include "core.h"
#include <stdexcept>
#include <cstdint>
#include <string>
#include <type_traits>

namespace Stalker::Bindings {

template <typename T>
struct TraitTag {
    using type = T;
};

// C++17 Template mapping trick to convert runtime ENUM to Compile-time ENUM wrapper
template <auto... Vals, typename ValueType, typename Func>
constexpr void dispatch_enum(ValueType runtime_val, Func&& f) {
    bool matched = ((runtime_val == Vals ? (f(std::integral_constant<decltype(Vals), Vals>{}), true) : false) || ...);
    if (!matched) throw std::invalid_argument("Invalid runtime trait configuration (value not supported or exposed).");
}

template <typename Func, typename... Ptrs>
inline void dispatch_execution(const ExecutionTraitPy& config, Func&& f, Ptrs... ptrs) {
    using namespace Stalker::Core;
    using namespace Stalker::Core::Config;

    switch (config.type) {
        case T_ExecTrait::Scalar: {
            if (config.enable_std) f(TraitTag<ExecutionTraitScalar<true>>{});
            else                   f(TraitTag<ExecutionTraitScalar<false>>{});
            break;
        }
        case T_ExecTrait::Unrolled: {
            dispatch_enum<size_t(1), size_t(2), size_t(4), size_t(8)>(config.unroll_factor, [&](auto unroll_ic) {
                f(TraitTag<ExecutionTraitUnrolled<unroll_ic.value>>{});
            });
            break;
        }
        case T_ExecTrait::SIMD: {
            // ── Resolve alignment from AlignmentPolicy + pointer inspection ──
            size_t min_alignment = (config.simd_type == Config::T_SIMD::AVX512) ? 64 : 32;
            bool is_aligned;

            switch (config.alignment_policy) {
                case AlignmentPolicy::Aligned: {
                    // User asserts alignment — validate pointers to prevent silent UB
                    if constexpr (sizeof...(Ptrs) > 0) {
                        uintptr_t combined = 0;
                        (..., (combined |= reinterpret_cast<uintptr_t>(ptrs)));
                        if (combined % min_alignment != 0) {
                            throw std::invalid_argument(
                                "AlignmentPolicy.Aligned requested but buffer addresses are not "
                                + std::to_string(min_alignment) + "-byte aligned. "
                                "Use AlignmentPolicy.Auto for runtime detection or ensure proper allocation.");
                        }
                    }
                    is_aligned = true;
                    break;
                }
                case AlignmentPolicy::Unaligned:
                    is_aligned = false;
                    break;
                case AlignmentPolicy::Auto:
                default: {
                    if constexpr (sizeof...(Ptrs) > 0) {
                        uintptr_t combined = 0;
                        (..., (combined |= reinterpret_cast<uintptr_t>(ptrs)));
                        is_aligned = (combined % min_alignment == 0);
                    } else {
                        is_aligned = false;
                    }
                    break;
                }
            }

            auto dispatch_simd = [&](auto simd_ic) {
                constexpr auto simd_val = simd_ic.value;
                dispatch_enum<true, false>(is_aligned, [&](auto align_ic) {
                    constexpr auto align_val = align_ic.value;
                    dispatch_enum<T_SIMDStore::Cached, T_SIMDStore::Streamed>(config.store_policy, [&](auto store_ic) {
                        constexpr auto store_val = store_ic.value;
                        if constexpr (!align_val && store_val == T_SIMDStore::Streamed) {
                            throw std::invalid_argument("Streamed memory stores inherently require aligned memory.");
                        }
                        dispatch_enum<T_PrefetchHints::HintNone, T_PrefetchHints::HintT0, T_PrefetchHints::HintT1, T_PrefetchHints::HintT2, T_PrefetchHints::HintNTA>(config.prefetch_hint, [&](auto prefetch_ic) {
                            constexpr auto prefetch_val = prefetch_ic.value;
                            dispatch_enum<T_ILPPolicy::Interleaved, T_ILPPolicy::Grouped>(config.ilp_policy, [&](auto ilp_ic) {
                                constexpr auto ilp_val = ilp_ic.value;
                                // Clamp to nearest supported unroll factor {1, 2, 4}
                                dispatch_enum<size_t(1), size_t(2), size_t(4)>(
                                    config.unroll_factor <= 1 ? size_t(1) : config.unroll_factor <= 2 ? size_t(2) : size_t(4),
                                    [&](auto unroll_ic) {
                                    constexpr auto unroll_val = unroll_ic.value;
                                    f(TraitTag<ExecutionTraitSIMD<simd_val, align_val, store_val, unroll_val, prefetch_val, ilp_val>>{});
                                });
                            });
                        });
                    });
                });
            };

            if (config.simd_type == Config::T_SIMD::AVX512) {
                #ifdef STALKER_SIMD_INSTRUCTION_SET_AVX512
                    dispatch_simd(std::integral_constant<T_SIMD, T_SIMD::AVX512>{});
                #else
                    throw std::runtime_error("AVX512 is not enabled in this build.");
                #endif
            } else {
                #ifdef STALKER_SIMD_INSTRUCTION_SET_AVX2
                    dispatch_simd(std::integral_constant<T_SIMD, T_SIMD::AVX2>{});
                #else
                    dispatch_simd(std::integral_constant<T_SIMD, DefaultSIMDType()>{});
                #endif
            }
            break;
        }
        case T_ExecTrait::None:
        default:
            f(TraitTag<DefaultExecutionTrait>{});
            break;
    }
}

} // namespace Stalker::Bindings
