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
set(PAPI_LIBDIR "/home/hal10000/code/papi/src/install")
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



# Data types iteration
set(STALKER_BENCH_TYPES "float;double" CACHE STRING "")
set(_bench_types_macro "")
foreach(t IN LISTS STALKER_BENCH_TYPES)
    set(_bench_types_macro "${_bench_types_macro} X(${t})")
endforeach()

# Vector sizes
# set(STALKER_BENCH_VECTOR_SIZES "256" CACHE STRING "Vector sizes to benchmark")
# Generate a dense, presentation-grade set of sizes across ranges
# Base: powers of two from 2^8 (256) up to <= 2,000,000,000
# Plus: in-between points at +25%, +50%, +75% of each power-of-two octave
# This yields 4 points per octave: 1.00x, 1.25x, 1.50x, 1.75x (all integers for exp>=2)
# ---- Tunables ----
set(_bench_start_elems 4096)           # first size
set(_bench_start_elems 100000)           # first size
set(_bench_max_elems   2000000000)     # cap (elements)
set(_bench_align       64)             # align up to multiple of 64
# ratio r = P/Q = 3/2
set(_P 3)
set(_Q 2)

# ---- Build geometric series ----
set(_bench_sizes_list "")
set(_cur ${_bench_start_elems})
set(_last 0)

while(TRUE)
  # align current up
  math(EXPR _cur_aligned "((${_cur} + ${_bench_align} - 1) / ${_bench_align}) * ${_bench_align}")

  # ensure strict increase after alignment
  if(_cur_aligned GREATER _bench_max_elems)
    break()
  endif()
  if(_cur_aligned GREATER _last)
    if(_bench_sizes_list)
      set(_bench_sizes_list "${_bench_sizes_list};${_cur_aligned}")
    else()
      set(_bench_sizes_list "${_cur_aligned}")
    endif()
    set(_last ${_cur_aligned})
  endif()

  # next = ceil(cur * P / Q)
  math(EXPR _next_raw "((${_cur} * ${_P}) + ${_Q} - 1) / ${_Q}")

  # guard against stall due to integer/align rounding
  if(_next_raw LESS_EQUAL ${_cur})
    math(EXPR _next_raw "${_cur} + ${_bench_align}")
  endif()

  set(_cur ${_next_raw})
endwhile()

# ---- Export to your bench config ----
set(STALKER_BENCH_VECTOR_SIZES "${_bench_sizes_list}" CACHE STRING "Geometric sizes (start=${_bench_start_elems}, ratio=${_P}/${_Q})")
string(REPLACE ";" ", " _bench_sizes_csv "${STALKER_BENCH_VECTOR_SIZES}")
list(LENGTH STALKER_BENCH_VECTOR_SIZES _bench_sizes_len)

# Unroll factors
# set(STALKER_BENCH_UNROLLS "1" CACHE STRING "Unroll factors")
set(STALKER_BENCH_UNROLLS "1" CACHE STRING "Unroll factors")
string(REPLACE ";" ", " _bench_unrolls_csv "${STALKER_BENCH_UNROLLS}")
list(LENGTH STALKER_BENCH_UNROLLS _bench_unrolls_len)

set(_bench_unrolls_macro "")
foreach(u IN LISTS STALKER_BENCH_UNROLLS)
  set(_bench_unrolls_macro "${_bench_unrolls_macro} X(${u})")
endforeach()

# ----- iterations -----
set(STALKER_BENCH_ITERATIONS 10 CACHE STRING "Benchmark repeats per case")
set(STALKER_BENCH_WARMUP_ITERATIONS 2 CACHE STRING "Warmup repeats per case")

# make them literals for C++
set(_bench_iterations "${STALKER_BENCH_ITERATIONS}")
set(_bench_warmup     "${STALKER_BENCH_WARMUP_ITERATIONS}")

# ----- memory ops -----
# set(STALKER_BENCH_MEMOPS "copy" CACHE STRING "Memory ops")
set(STALKER_BENCH_MEMOPS "setValue" CACHE STRING "Memory ops")
# set(STALKER_BENCH_MEMOPS "copy;setValue;setZero" CACHE STRING "Memory ops")

set(_bench_memops_csv_quoted "${STALKER_BENCH_MEMOPS}")
string(REPLACE ";" "\", \"" _bench_memops_csv_quoted "${_bench_memops_csv_quoted}")
set(_bench_memops_csv_quoted "\"${_bench_memops_csv_quoted}\"")

list(LENGTH STALKER_BENCH_MEMOPS _bench_memops_len)


# ----- math ops -----
# Names for mathematical operations benchmarks (used by BenchmarkConfig.hpp)
set(STALKER_BENCH_MATHOPS "scale" CACHE STRING "Math operations")

set(_bench_mathops_csv_quoted "${STALKER_BENCH_MATHOPS}")
string(REPLACE ";" "\", \"" _bench_mathops_csv_quoted "${_bench_mathops_csv_quoted}")
set(_bench_mathops_csv_quoted "\"${_bench_mathops_csv_quoted}\"")

list(LENGTH STALKER_BENCH_MATHOPS _bench_mathops_len)


# Generate header into the build dir
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/BenchmarkConfig.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/BenchmarkConfig.hpp
  @ONLY
)

# Make the generated header visible to Benchmarks
target_include_directories(Benchmarks PRIVATE ${CMAKE_CURRENT_BINARY_DIR})