# Options: make a dep hard-required or optional

option(STALKER_BENCH_EIGEN_ENABLE     "Fail if Eigen is missing"     ON)
option(STALKER_BENCH_OPENBLAS_ENABLE  "Fail if OpenBLAS is missing"  ON)
option(STALKER_BENCH_PAPI_ENABLE      "Fail if PAPI is missing"      ON)

add_executable(Benchmarks ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_compile_features(Benchmarks PRIVATE cxx_std_20)

# ---------------- Eigen ----------------
set(HAVE_EIGEN 0)
find_package(Eigen3 QUIET)
if(Eigen3_FOUND)
    target_link_libraries(Benchmarks PRIVATE Eigen3::Eigen)
    set(HAVE_EIGEN 1)
    message(STATUS "Eigen3 found and linked successfully.")
elseif(STALKER_BENCH_EIGEN_ENABLE)
    message(FATAL_ERROR "Eigen3 required but not found.")
endif()
target_compile_definitions(Benchmarks PRIVATE STALKER_BENCH_EIGEN_AVAILABLE=${HAVE_EIGEN})

# ---------------- OpenBLAS ----------------
set(HAVE_OPENBLAS 0)
find_package(OpenBLAS QUIET)
if(TARGET OpenBLAS::OpenBLAS)
    target_link_libraries(Benchmarks PRIVATE OpenBLAS::OpenBLAS)
    set(HAVE_OPENBLAS 1)
else()
    # fallback to pkg-config: openblas or blas depending on distro
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(OBLAS QUIET openblas)
        if(OBLAS_FOUND)
            target_include_directories(Benchmarks PRIVATE ${OBLAS_INCLUDE_DIRS})
            target_link_directories(Benchmarks PRIVATE ${OBLAS_LIBRARY_DIRS})
            target_link_libraries(Benchmarks PRIVATE ${OBLAS_LINK_LIBRARIES})
            set(HAVE_OPENBLAS 1)
            message(STATUS "OpenBLAS found and linked successfully.")
        else()
            pkg_check_modules(BLASPC QUIET blas)
            if(BLASPC_FOUND)
                target_include_directories(Benchmarks PRIVATE ${BLASPC_INCLUDE_DIRS})
                target_link_directories(Benchmarks PRIVATE ${BLASPC_LIBRARY_DIRS})
                target_link_libraries(Benchmarks PRIVATE ${BLASPC_LINK_LIBRARIES})
                set(HAVE_OPENBLAS 1)
                message(STATUS "OpenBLAS found and linked successfully.")
            endif()
        endif()
    endif()
    if(NOT HAVE_OPENBLAS AND STALKER_BENCH_OPENBLAS_ENABLE)
        message(FATAL_ERROR "OpenBLAS (or BLAS via pkg-config) required but not found.")
    endif()
endif()
target_compile_definitions(Benchmarks PRIVATE STALKER_BENCH_OPENBLAS_AVAILABLE=${HAVE_OPENBLAS})

# ---------------- PAPI ----------------
set(HAVE_PAPI 0)
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PAPI QUIET papi)
    if(PAPI_FOUND)
        target_include_directories(Benchmarks PRIVATE ${PAPI_INCLUDE_DIRS})
        target_link_directories(Benchmarks PRIVATE ${PAPI_LIBRARY_DIRS})
        target_link_libraries(Benchmarks PRIVATE ${PAPI_LINK_LIBRARIES})
        if(PAPI_LIBDIR)
            set_target_properties(Benchmarks PROPERTIES BUILD_RPATH "${PAPI_LIBDIR}")
        endif()
        set(HAVE_PAPI 1)
        message(STATUS "PAPI found and linked successfully.")
    endif()
endif()
if(NOT HAVE_PAPI AND STALKER_BENCH_PAPI_ENABLE)
    message(FATAL_ERROR "PAPI required but not found.")
endif()
target_compile_definitions(Benchmarks PRIVATE STALKER_BENCH_PAPI_AVAILABLE=${HAVE_PAPI})

# ---------------- Your core lib ----------------
target_link_libraries(Benchmarks PRIVATE Stalker)


# Compile-time sizes
set(STALKER_BENCH_VECTOR_SIZES "256;512;1024;2048;4096" CACHE STRING "Vector sizes to benchmark")

# Prepare substitutions for the template
string(REPLACE ";" ", " _bench_sizes_csv "${STALKER_BENCH_VECTOR_SIZES}")
list(LENGTH STALKER_BENCH_VECTOR_SIZES _bench_sizes_len)

set(STALKER_BENCH_TYPES "float;double;int" CACHE STRING "")
set(_bench_types_macro "")
foreach(t IN LISTS STALKER_BENCH_TYPES)
  set(_bench_types_macro "${_bench_types_macro} X(${t})")
endforeach()

# Generate header into the build dir
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Config.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/BenchConfig.hpp
  @ONLY
)

# Make the generated header visible to Benchmarks
target_include_directories(Benchmarks PRIVATE ${CMAKE_CURRENT_BINARY_DIR})