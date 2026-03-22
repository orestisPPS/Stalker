#include "core.h"
#include <nanobind/stl/string.h>
#include <string>

namespace nb = nanobind;
using namespace Stalker::Core;

void bind_core(nb::module_& m) {
    nb::module_ core_mod = m.def_submodule("core", "Stalker Core operations and traits");
    
    // Bind the actual Core enums mapping directly to C++
    nb::enum_<T_ExecTrait>(core_mod, "ExecTrait")
        .value("Scalar", T_ExecTrait::Scalar)
        .value("Unrolled", T_ExecTrait::Unrolled)
        .value("SIMD", T_ExecTrait::SIMD)
        .value("None", T_ExecTrait::None)
        .export_values();

    // Bind SIMD types
    nb::enum_<Config::T_SIMD>(core_mod, "SIMDType")
        .value("None", Config::T_SIMD::None)
        .value("AVX2", Config::T_SIMD::AVX2)
        .value("AVX512", Config::T_SIMD::AVX512)
        .export_values();

    // Bind SIMD specific configurations
    nb::enum_<Config::T_SIMDStore>(core_mod, "SIMDStorePolicy")
        .value("Cached", Config::T_SIMDStore::Cached)
        .value("Streamed", Config::T_SIMDStore::Streamed)
        .export_values();

    nb::enum_<Config::T_ILPPolicy>(core_mod, "ILPPolicy")
        .value("Interleaved", Config::T_ILPPolicy::Interleaved)
        .value("Grouped", Config::T_ILPPolicy::Grouped)
        .export_values();

    nb::enum_<Config::T_PrefetchHints>(core_mod, "PrefetchHint")
        .value("None", Config::T_PrefetchHints::HintNone)
        .value("T0", Config::T_PrefetchHints::HintT0)
        .value("T1", Config::T_PrefetchHints::HintT1)
        .value("T2", Config::T_PrefetchHints::HintT2)
        .value("NTA", Config::T_PrefetchHints::HintNTA)
        .export_values();

    // Alignment dispatch policy — mirrors ExecutionTraitSIMD<..., Aligned, ...>
    nb::enum_<AlignmentPolicy>(core_mod, "AlignmentPolicy", "Policy defining hardware buffer alignment validations dictating whether vectorized load and store operations are guaranteed perfectly aligned 256-bit or 512-bit boundaries or if unaligned instructions are necessary.")
        .value("Auto", AlignmentPolicy::Auto, "Checks runtime pointers. If aligned, dispatches aligned instruction sets safely. Otherwise dispatches unaligned gracefully.")
        .value("Aligned", AlignmentPolicy::Aligned, "Forcefully assumes data alignment is natively exact to SIMD specifications at execution time. Throws a Python runtime error prior to buffer bounds dispatching if addresses map improperly mitigating undocumented hardware faults.")
        .value("Unaligned", AlignmentPolicy::Unaligned, "Implicitly enforces unaligned intrinsic processing loads and stores universally unconditionally allowing vectors stored randomly in heap segments regardless of block start offset.")
        .export_values();
        
    // Bind the unified Runtime Config class, allowing it to act like polymorphic policy classes
    nb::class_<ExecutionTraitPy>(core_mod, "ExecutionConfig")
        .def(nb::init<>())
        .def_rw("type", &ExecutionTraitPy::type)
        .def_rw("enable_std", &ExecutionTraitPy::enable_std)
        .def_rw("unroll_factor", &ExecutionTraitPy::unroll_factor)
        .def_rw("simd_type", &ExecutionTraitPy::simd_type)
        .def_rw("store_policy", &ExecutionTraitPy::store_policy)
        .def_rw("ilp_policy", &ExecutionTraitPy::ilp_policy)
        .def_rw("prefetch_hint", &ExecutionTraitPy::prefetch_hint)
        .def_rw("alignment_policy", &ExecutionTraitPy::alignment_policy)
        .def_static("Default", &ExecutionTraitPy::Default, "Use the automatically deduced C++ backend trait")
        .def_static("Scalar", &ExecutionTraitPy::Scalar, "Force classical execution (No vectorization)", nb::arg("enable_std") = true)
        .def_static("Unrolled", &ExecutionTraitPy::Unrolled, "Force unrolled loop processing", nb::arg("unroll") = 1)
        .def_static("SIMD", &ExecutionTraitPy::SIMD, "Force SIMD vectorization",
            nb::arg("simd_type") = Config::T_SIMD::AVX2,
            nb::arg("unroll") = 1,
            nb::arg("store") = Config::T_SIMDStore::Cached,
            nb::arg("prefetch") = Config::T_PrefetchHints::HintNone,
            nb::arg("ilp") = Config::T_ILPPolicy::Interleaved,
            nb::arg("aligned") = AlignmentPolicy::Auto)
        .def("__repr__", [](const ExecutionTraitPy& c) {
            std::string type_name;
            switch(c.type) {
                case T_ExecTrait::Scalar: type_name = "Scalar"; break;
                case T_ExecTrait::Unrolled: type_name = "Unrolled"; break;
                case T_ExecTrait::SIMD: type_name = "SIMD"; break;
                case T_ExecTrait::None: type_name = "Default"; break;
            }
            std::string simd_name = "None";
            if (c.simd_type == Config::T_SIMD::AVX2) simd_name = "AVX2";
            else if (c.simd_type == Config::T_SIMD::AVX512) simd_name = "AVX512";
            
            std::string store_name = (c.store_policy == Config::T_SIMDStore::Cached) ? "Cached" : "Streamed";
            std::string ilp_name = (c.ilp_policy == Config::T_ILPPolicy::Interleaved) ? "Interleaved" : "Grouped";
            
            std::string align_name;
            switch(c.alignment_policy) {
                case AlignmentPolicy::Auto:      align_name = "Auto"; break;
                case AlignmentPolicy::Aligned:   align_name = "Aligned"; break;
                case AlignmentPolicy::Unaligned: align_name = "Unaligned"; break;
            }
            
            return "<ExecutionConfig backend=" + type_name + 
                   " std=" + (c.enable_std ? "True" : "False") +
                   " simd=" + simd_name +
                   " aligned=" + align_name + ">";
        });
}