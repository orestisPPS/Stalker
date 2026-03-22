#include "memory.h"
#include "../core/core.h"
#include "../core/dispatch.h"
#include <Stalker/Memory/MemoryOperations.h>
#include <Stalker/Memory/Allocators.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>
#include <cstring>
#include <stdexcept>
#include <string>

namespace nb = nanobind;
using namespace Stalker::Memory;
using namespace Stalker::Core;
using namespace Stalker::Bindings;

template <typename T, size_t Alignment>
nb::object alloc_impl(size_t size, bool zero) {
    T* data = createAlignedPtr<T, Alignment>(size);
    if (zero) {
        std::memset(static_cast<void*>(data), 0, size * sizeof(T));
    }
    nb::capsule deleter(data, [](void* p) noexcept {
        AlignedDeleter<T, Alignment>{}(static_cast<T*>(p));
    });
    size_t shape[1] = { size };
    return nb::cast(nb::ndarray<nb::numpy, T, nb::c_contig>(data, 1, shape, deleter));
}

template<typename T>
nb::object make_aligned_t(size_t size, size_t alignment, bool zero) {
    if (alignment == 64) return alloc_impl<T, 64>(size, zero);
    else if (alignment == 32) return alloc_impl<T, 32>(size, zero);
    else if (alignment == 16) return alloc_impl<T, 16>(size, zero);
    else if (alignment == 128) return alloc_impl<T, 128>(size, zero);
    else throw std::invalid_argument("Unsupported alignment. Choose 16, 32, 64, or 128.");
}

std::string get_dtype_name(nb::handle dtype) {
    if(dtype.is_none()) return "float32";
    if (nb::isinstance<nb::str>(dtype)) {
        return nb::cast<std::string>(dtype);
    } 
    if (nb::hasattr(dtype, "__name__")) {
        return nb::cast<std::string>(nb::getattr(dtype, "__name__"));
    } 
    if (nb::hasattr(dtype, "name")) {
        return nb::cast<std::string>(nb::getattr(dtype, "name"));
    }
    return "float32"; // Safe default
}

nb::object dispatch_alloc(size_t size, size_t alignment, nb::handle dtype, bool zero) {
    std::string dt_name = get_dtype_name(dtype);
    if (dt_name == "float32" || dt_name == "float") return make_aligned_t<float>(size, alignment, zero);
    if (dt_name == "float64" || dt_name == "double" || dt_name == "float_") return make_aligned_t<double>(size, alignment, zero);
    if (dt_name == "int32" || dt_name == "intc" || dt_name == "int") return make_aligned_t<int>(size, alignment, zero);
    if (dt_name == "uint32" || dt_name == "uint") return make_aligned_t<unsigned int>(size, alignment, zero);
    if (dt_name == "int16" || dt_name == "short") return make_aligned_t<short>(size, alignment, zero);
    throw std::invalid_argument("Unsupported dtype: " + dt_name);
}

template <typename T>
void bind_memory_ops(nb::module_& m) {
    m.def("copy", [](nb::ndarray<T, nb::c_contig> dest,
                     nb::ndarray<const T, nb::c_contig> src,
                     const ExecutionTraitPy& config) {
        if (dest.size() != src.size()) throw std::invalid_argument("Array sizes must match");
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type; 
            MemoryOperations::copy<T, Trait>(dest.size(), dest.data(), src.data());
        }, dest.data(), src.data());
    }, "Copy data from src to dest",
       nb::arg("dest"), nb::arg("src"), nb::arg("config") = ExecutionTraitPy::Default());

    m.def("swap", [](nb::ndarray<T, nb::c_contig> data1,
                     nb::ndarray<T, nb::c_contig> data2,
                     const ExecutionTraitPy& config) {
        if (data1.size() != data2.size()) throw std::invalid_argument("Array sizes must match");
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type; 
            MemoryOperations::swap<T, Trait>(data1.size(), data1.data(), data2.data());
        }, data1.data(), data2.data());
    }, "Swap data between data1 and data2",
       nb::arg("data1"), nb::arg("data2"), nb::arg("config") = ExecutionTraitPy::Default());

    m.def("set_value", [](nb::ndarray<T, nb::c_contig> dest,
                          T value,
                          const ExecutionTraitPy& config) {
        dispatch_execution(config, [&](auto tag) {
            using Trait = typename decltype(tag)::type; 
            MemoryOperations::setValue<T, Trait>(dest.size(), dest.data(), value);
        }, dest.data());
    }, "Set all elements in array to given value",
       nb::arg("dest"), nb::arg("value"), nb::arg("config") = ExecutionTraitPy::Default());
}

void bind_memory(nb::module_& m) {
    nb::module_ mem_mod = m.def_submodule("memory", "Stalker Memory operations and allocators");

    mem_mod.def("empty_aligned", [](size_t size, size_t alignment, nb::handle dtype) {
        return dispatch_alloc(size, alignment, dtype, false);
    }, "Allocate uninitialized aligned memory array", 
       nb::arg("size"), nb::arg("alignment") = 32, nb::arg("dtype") = nb::none());

    mem_mod.def("zeros_aligned", [](size_t size, size_t alignment, nb::handle dtype) {
        return dispatch_alloc(size, alignment, dtype, true);
    }, "Allocate zero-initialized aligned memory array", 
       nb::arg("size"), nb::arg("alignment") = 32, nb::arg("dtype") = nb::none());

    // Bind operations generically leveraging nanobind's overloading
    bind_memory_ops<double>(mem_mod);
    bind_memory_ops<float>(mem_mod);
    bind_memory_ops<int>(mem_mod);
    bind_memory_ops<unsigned int>(mem_mod);
    bind_memory_ops<short>(mem_mod);
}
