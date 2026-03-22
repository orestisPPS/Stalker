#include <nanobind/nanobind.h>
#include "core/core.h"
#include "mathematics/vector_math.h"
#include "memory/memory.h"

namespace nb = nanobind;

// Define the root Python module named 'stalker_ext'
NB_MODULE(stalker_ext, m) {
    m.doc() = "Stalker Math HPC Python Bindings";

    // Bind sub-modules mirroring project structure
    bind_core(m);
    bind_vector_math(m);
    bind_memory(m);
    
    // A simple test function just to ensure bindings are working:
    m.def("hello", []() { return "Hello from Stalker C++!"; });
}
