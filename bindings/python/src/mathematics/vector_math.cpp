#include <stdexcept>
#include <string>
#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>
#include "vector_math.h"
#include "../core/core.h"
#include "../core/dispatch.h"
#include <Stalker/Mathematics/Vector/VectorMath.h>

namespace nb = nanobind;
using namespace Stalker::Core;
using namespace Stalker::Mathematics;
using namespace Stalker::Bindings;

template <typename T>
void bind_vector_math_impl(nb::module_& m) {
    // Add
    m.def("add", [](
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> a,
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> b,
        nb::ndarray<T, nb::c_contig, nb::device::cpu> result,
        const ExecutionTraitPy& config
    ) {
        if (a.size() != b.size() || a.size() != result.size()) throw std::invalid_argument("Array sizes must match exactly.");
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            VectorMath::add<T, Trait>(a.size(), a.data(), b.data(), result.data());
        }, a.data(), b.data(), result.data());
    }, "Add two vectors: result = a + b",
    nb::arg("a"), nb::arg("b"), nb::arg("result"), nb::arg("config") = ExecutionTraitPy::Default());

    // Add with scalars
    m.def("add", [](
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> a,
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> b,
        nb::ndarray<T, nb::c_contig, nb::device::cpu> result,
        T scalarA, T scalarB,
        const ExecutionTraitPy& config
    ) {
        if (a.size() != b.size() || a.size() != result.size()) throw std::invalid_argument("Array sizes must match exactly.");
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            VectorMath::add<T, Trait>(a.size(), a.data(), b.data(), result.data(), scalarA, scalarB);
        }, a.data(), b.data(), result.data());
    }, "Add two vectors with scalars",
    nb::arg("a"), nb::arg("b"), nb::arg("result"), nb::arg("scalarA"), nb::arg("scalarB"), nb::arg("config") = ExecutionTraitPy::Default());

    // Axpy
    m.def("axpy", [](
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> a,
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> b,
        nb::ndarray<T, nb::c_contig, nb::device::cpu> result,
        T scalar,
        const ExecutionTraitPy& config
    ) {
        if (a.size() != b.size() || a.size() != result.size()) throw std::invalid_argument("Array sizes must match exactly.");
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            VectorMath::axpy<T, Trait>(a.size(), a.data(), b.data(), result.data(), scalar);
        }, a.data(), b.data(), result.data());
    }, "AXPY: result = a * scalar + b",
    nb::arg("a"), nb::arg("b"), nb::arg("result"), nb::arg("scalar"), nb::arg("config") = ExecutionTraitPy::Default());

    // Subtract
    m.def("subtract", [](
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> a,
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> b,
        nb::ndarray<T, nb::c_contig, nb::device::cpu> result,
        const ExecutionTraitPy& config
    ) {
        if (a.size() != b.size() || a.size() != result.size()) throw std::invalid_argument("Array sizes must match exactly.");
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            VectorMath::subtract<T, Trait>(a.size(), a.data(), b.data(), result.data());
        }, a.data(), b.data(), result.data());
    }, "Subtract two vectors: result = a - b",
    nb::arg("a"), nb::arg("b"), nb::arg("result"), nb::arg("config") = ExecutionTraitPy::Default());

    // Subtract with scalars
    m.def("subtract", [](
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> a,
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> b,
        nb::ndarray<T, nb::c_contig, nb::device::cpu> result,
        T scalarA, T scalarB,
        const ExecutionTraitPy& config
    ) {
        if (a.size() != b.size() || a.size() != result.size()) throw std::invalid_argument("Array sizes must match exactly.");
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            VectorMath::subtract<T, Trait>(a.size(), a.data(), b.data(), result.data(), scalarA, scalarB);
        }, a.data(), b.data(), result.data());
    }, "Subtract two vectors with scalars",
    nb::arg("a"), nb::arg("b"), nb::arg("result"), nb::arg("scalarA"), nb::arg("scalarB"), nb::arg("config") = ExecutionTraitPy::Default());

    // Multiply
    m.def("multiply", [](
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> a,
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> b,
        nb::ndarray<T, nb::c_contig, nb::device::cpu> result,
        const ExecutionTraitPy& config
    ) {
        if (a.size() != b.size() || a.size() != result.size()) throw std::invalid_argument("Array sizes must match exactly.");
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            VectorMath::multiply<T, Trait>(a.size(), a.data(), b.data(), result.data());
        }, a.data(), b.data(), result.data());
    }, "Multiply two vectors: result = a * b",
    nb::arg("a"), nb::arg("b"), nb::arg("result"), nb::arg("config") = ExecutionTraitPy::Default());

    // Multiply with scalars
    m.def("multiply", [](
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> a,
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> b,
        nb::ndarray<T, nb::c_contig, nb::device::cpu> result,
        T scalarA, T scalarB,
        const ExecutionTraitPy& config
    ) {
        if (a.size() != b.size() || a.size() != result.size()) throw std::invalid_argument("Array sizes must match exactly.");
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            VectorMath::multiply<T, Trait>(a.size(), a.data(), b.data(), result.data(), scalarA, scalarB);
        }, a.data(), b.data(), result.data());
    }, "Multiply two vectors with scalars",
    nb::arg("a"), nb::arg("b"), nb::arg("result"), nb::arg("scalarA"), nb::arg("scalarB"), nb::arg("config") = ExecutionTraitPy::Default());

    // Scale
    m.def("scale", [](
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> data,
        nb::ndarray<T, nb::c_contig, nb::device::cpu> result,
        T scalar,
        const ExecutionTraitPy& config
    ) {
        if (data.size() != result.size()) throw std::invalid_argument("Array sizes must match exactly.");
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            VectorMath::scale<T, Trait>(data.size(), data.data(), result.data(), scalar);
        }, data.data(), result.data());
    }, "Scale vector: result = data * scalar",
    nb::arg("data"), nb::arg("result"), nb::arg("scalar"), nb::arg("config") = ExecutionTraitPy::Default());

    // Scale in-place
    m.def("scale", [](
        nb::ndarray<T, nb::c_contig, nb::device::cpu> data,
        T scalar,
        const ExecutionTraitPy& config
    ) {
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            VectorMath::scale<T, Trait>(data.size(), data.data(), scalar);
        }, data.data());
    }, "Scale vector in-place: data = data * scalar",
    nb::arg("data"), nb::arg("scalar"), nb::arg("config") = ExecutionTraitPy::Default());

    // AddConstant
    m.def("addConstant", [](
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> data,
        nb::ndarray<T, nb::c_contig, nb::device::cpu> result,
        T constant,
        const ExecutionTraitPy& config
    ) {
        if (data.size() != result.size()) throw std::invalid_argument("Array sizes must match exactly.");
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            VectorMath::addConstant<T, Trait>(data.size(), data.data(), result.data(), constant);
        }, data.data(), result.data());
    }, "Add constant to vector: result = data + constant",
    nb::arg("data"), nb::arg("result"), nb::arg("constant"), nb::arg("config") = ExecutionTraitPy::Default());

    // AddConstant in-place
    m.def("addConstant", [](
        nb::ndarray<T, nb::c_contig, nb::device::cpu> data,
        T constant,
        const ExecutionTraitPy& config
    ) {
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            VectorMath::addConstant<T, Trait>(data.size(), data.data(), constant);
        }, data.data());
    }, "Add constant to vector in-place: data = data + constant",
    nb::arg("data"), nb::arg("constant"), nb::arg("config") = ExecutionTraitPy::Default());

    // Sum
    m.def("sum", [](
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> data,
        const ExecutionTraitPy& config
    ) -> T {
        T sum_result = 0;
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            sum_result = VectorMath::sum<T, Trait>(data.size(), data.data());
        }, data.data());
        return sum_result;
    }, "Sum elements of a vector",
    nb::arg("data"), nb::arg("config") = ExecutionTraitPy::Default());

    // Dot product
    m.def("dot", [](
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> a,
        nb::ndarray<const T, nb::c_contig, nb::device::cpu> b,
        const ExecutionTraitPy& config
    ) -> T {
        if (a.size() != b.size()) throw std::invalid_argument("Array sizes must match exactly.");
        T dot_result = 0;
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type;
            dot_result = VectorMath::dot<T, Trait>(a.size(), a.data(), b.data());
        }, a.data(), b.data());
        return dot_result;
    }, "Dot product of two vectors",
    nb::arg("a"), nb::arg("b"), nb::arg("config") = ExecutionTraitPy::Default());
}

void bind_vector_math(nb::module_& m) {
    nb::module_ vmath_mod = m.def_submodule("vector_math", "Stalker VectorMath operations");
    
    bind_vector_math_impl<double>(vmath_mod);
    bind_vector_math_impl<float>(vmath_mod);
    bind_vector_math_impl<int>(vmath_mod);
    bind_vector_math_impl<unsigned int>(vmath_mod);
    bind_vector_math_impl<short>(vmath_mod);
}
