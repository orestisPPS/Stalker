cmake_minimum_required(VERSION 3.18)

find_package(Python 3.8 COMPONENTS Interpreter Development.Module REQUIRED)
find_package(nanobind CONFIG REQUIRED)

# nanobind_add_module creates a target for a Python extension module
nanobind_add_module(stalker_ext LTO
    src/main.cpp
    src/core/core.cpp
    src/mathematics/vector_math.cpp
    src/memory/memory.cpp
)

# Enable highest optimization for the binding targets
set_target_properties(stalker_ext PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    INTERPROCEDURAL_OPTIMIZATION ON
)

# Link against only the required Stalker sub-modules
target_link_libraries(stalker_ext PRIVATE StalkerMemory StalkerMathematics)

# For scikit-build-core to appropriately place the built shared object
# inside the Python package directory `bindings/python/stalker/` during the build
install(TARGETS stalker_ext DESTINATION stalker)
