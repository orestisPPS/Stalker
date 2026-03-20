# Options: make a dep hard-required or optional

option(STALKER_BENCH_EIGEN_ENABLE     "Fail if Eigen is missing"     ON)
option(STALKER_BENCH_OPENBLAS_ENABLE  "Fail if OpenBLAS is missing"  ON)
option(STALKER_BENCH_PAPI_ENABLE      "Fail if PAPI is missing"      ON)

add_executable(Benchmarks ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_compile_features(Benchmarks PRIVATE cxx_std_17)
find_package(Threads REQUIRED)
target_link_libraries(Benchmarks PRIVATE Threads::Threads)

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



#############################
# Benchmark configuration ingestion
#############################
# Optional: slave thread id for affinity pinning
set(_bench_slave_thread_id 0)
if(DEFINED STALKER_BENCH_SLAVE_THREAD_ID)
  if(NOT STALKER_BENCH_SLAVE_THREAD_ID MATCHES "^[0-9]+$")
    message(FATAL_ERROR "STALKER_BENCH_SLAVE_THREAD_ID must be a non-negative integer")
  endif()
  set(_bench_slave_thread_id ${STALKER_BENCH_SLAVE_THREAD_ID})
endif()

# Threads: mirror Stalker core thread count for Eigen/BLAS usage in benchmarks
set(_bench_num_threads 1)
if(DEFINED STALKER_THREADING_NUM_THREADS)
  if(NOT STALKER_THREADING_NUM_THREADS MATCHES "^[0-9]+$")
    message(FATAL_ERROR "STALKER_THREADING_NUM_THREADS must be a non-negative integer (got '${STALKER_THREADING_NUM_THREADS}')")
  endif()
  if(STALKER_THREADING_NUM_THREADS LESS 1)
    set(_bench_num_threads 1)
  else()
    set(_bench_num_threads ${STALKER_THREADING_NUM_THREADS})
  endif()
endif()


# Require user-specified benchmark configuration (no implicit defaults)
if(NOT DEFINED STALKER_BENCH_TYPES OR STALKER_BENCH_TYPES STREQUAL "")
  message(FATAL_ERROR "STALKER_BENCH_TYPES must be set (e.g. -DSTALKER_BENCH_TYPES=double;float)")
endif()
set(_bench_types_macro "")
foreach(t IN LISTS STALKER_BENCH_TYPES)
  set(_bench_types_macro "${_bench_types_macro} X(${t})")
endforeach()

# ----- Vector sizes -----
# Accept a single combined list (computed by the Python driver); CMake validates and prints.
if(NOT DEFINED STALKER_BENCH_VECTOR_SIZES OR STALKER_BENCH_VECTOR_SIZES STREQUAL "")
  message(FATAL_ERROR "No vector sizes provided. Set STALKER_BENCH_VECTOR_SIZES.")
endif()

set(_all_sizes ${STALKER_BENCH_VECTOR_SIZES})
list(REMOVE_DUPLICATES _all_sizes)
list(SORT _all_sizes COMPARE NATURAL)

string(REPLACE ";" ", " _bench_sizes_csv "${_all_sizes}")
list(LENGTH _all_sizes _bench_sizes_len)
if(_bench_sizes_len LESS 1)
  message(FATAL_ERROR "Vector sizes: must contain at least one size")
endif()
set(_seen_sizes "")
foreach(sz IN LISTS _all_sizes)
  if(NOT sz MATCHES "^[0-9]+$")
    message(FATAL_ERROR "Vector sizes: '${sz}' is not an integer")
  endif()
  if(sz STREQUAL "0")
    message(FATAL_ERROR "Vector sizes: size must be > 0 (got 0)")
  endif()
  # Removed 32-bit limit check to allow >4GB allocations (e.g. 64GB RAM tests)
  if(";${_seen_sizes};" MATCHES ";${sz};")
    message(FATAL_ERROR "Vector sizes: duplicate size '${sz}'")
  endif()
  set(_seen_sizes "${_seen_sizes};${sz}")
endforeach()

# Export a normalized combined list for downstream header generation
set(STALKER_BENCH_VECTOR_SIZES "${_all_sizes}")

# ----- Matrix sizes -----
if(NOT DEFINED STALKER_BENCH_MATRIX_SIZES OR STALKER_BENCH_MATRIX_SIZES STREQUAL "")
  set(STALKER_BENCH_MATRIX_SIZES "0") 
endif()

set(_all_matrix_sizes ${STALKER_BENCH_MATRIX_SIZES})
list(REMOVE_DUPLICATES _all_matrix_sizes)
list(SORT _all_matrix_sizes COMPARE NATURAL)

string(REPLACE ";" ", " _bench_matrix_sizes_csv "${_all_matrix_sizes}")
list(LENGTH _all_matrix_sizes _bench_matrix_sizes_len)

# Rough memory footprint validation (max element size heuristic)
set(_type_size_max 0)
foreach(t IN LISTS STALKER_BENCH_TYPES)
  if(t STREQUAL "double")
    set(_ts 8)
  elseif(t STREQUAL "float")
    set(_ts 4)
  elseif(t STREQUAL "int")
    set(_ts 4)
  elseif(t STREQUAL "long" OR t STREQUAL "long long")
    set(_ts 8)
  else()
    set(_ts 8)
  endif()
  if(_ts GREATER _type_size_max)
    set(_type_size_max ${_ts})
  endif()
endforeach()
math(EXPR _warn_bytes_threshold "64 * 1024 * 1024 * 1024")
foreach(sz IN LISTS STALKER_BENCH_VECTOR_SIZES)
  math(EXPR _footprint "${sz} * ${_type_size_max}")
  if(_footprint GREATER _warn_bytes_threshold)
    math(EXPR _footprint_mb "${_footprint} / (1024 * 1024)")
    message(WARNING "Benchmark footprint large (~${_footprint_mb} MiB for size ${sz}); consider reducing sizes.")
  endif()
endforeach()

# ----- Unroll factors -----
if(NOT DEFINED STALKER_BENCH_UNROLLS OR STALKER_BENCH_UNROLLS STREQUAL "")
  message(FATAL_ERROR "STALKER_BENCH_UNROLLS must be set (e.g. -DSTALKER_BENCH_UNROLLS=1;2;4)")
endif()
string(REPLACE ";" ", " _bench_unrolls_csv "${STALKER_BENCH_UNROLLS}")
list(LENGTH STALKER_BENCH_UNROLLS _bench_unrolls_len)
if(_bench_unrolls_len LESS 1)
  message(FATAL_ERROR "STALKER_BENCH_UNROLLS: must contain at least one factor")
endif()
set(_seen_unrolls "")
foreach(uf IN LISTS STALKER_BENCH_UNROLLS)
  if(NOT uf MATCHES "^[0-9]+$")
    message(FATAL_ERROR "STALKER_BENCH_UNROLLS: '${uf}' is not an integer")
  endif()
  if(uf STREQUAL "0")
    message(FATAL_ERROR "STALKER_BENCH_UNROLLS: unroll must be > 0")
  endif()
  if(";${_seen_unrolls};" MATCHES ";${uf};")
    message(FATAL_ERROR "STALKER_BENCH_UNROLLS: duplicate factor '${uf}'")
  endif()
  if(uf GREATER 64)
    message(FATAL_ERROR "STALKER_BENCH_UNROLLS: '${uf}' exceeds conservative upper limit (64)")
  endif()
  set(_seen_unrolls "${_seen_unrolls};${uf}")
endforeach()

set(_bench_unrolls_macro "")
foreach(u IN LISTS STALKER_BENCH_UNROLLS)
  set(_bench_unrolls_macro "${_bench_unrolls_macro} X(${u})")
endforeach()

# ----- Store Policies -----
if(NOT DEFINED STALKER_BENCH_STORE_POLICIES OR STALKER_BENCH_STORE_POLICIES STREQUAL "")
  set(STALKER_BENCH_STORE_POLICIES "cache") # Default
endif()

set(_bench_store_policies_macro "")
foreach(sp IN LISTS STALKER_BENCH_STORE_POLICIES)
  if(sp STREQUAL "stream")
    set(_bench_store_policies_macro "${_bench_store_policies_macro} X(Stalker::Core::Config::T_SIMDStore::Streamed)")
  elseif(sp STREQUAL "cache")
    set(_bench_store_policies_macro "${_bench_store_policies_macro} X(Stalker::Core::Config::T_SIMDStore::Cached)")
  else()
    message(FATAL_ERROR "Unknown store policy: '${sp}'. Supported: 'stream', 'cache'")
  endif()
endforeach()

# ----- ILP Policies -----
if(NOT DEFINED STALKER_BENCH_ILP_POLICIES OR STALKER_BENCH_ILP_POLICIES STREQUAL "")
  set(STALKER_BENCH_ILP_POLICIES "interleaved") # Default
endif()

set(_bench_ilp_policies_macro "")
foreach(ip IN LISTS STALKER_BENCH_ILP_POLICIES)
  if(ip STREQUAL "interleaved")
    set(_bench_ilp_policies_macro "${_bench_ilp_policies_macro} X(Stalker::Core::Config::T_ILPPolicy::Interleaved)")
  elseif(ip STREQUAL "grouped")
    set(_bench_ilp_policies_macro "${_bench_ilp_policies_macro} X(Stalker::Core::Config::T_ILPPolicy::Grouped)")
  else()
    message(FATAL_ERROR "Unknown ILP policy: '${ip}'. Supported: 'interleaved', 'grouped'")
  endif()
endforeach()

# ----- iterations -----
if(NOT DEFINED STALKER_BENCH_ITERATIONS OR STALKER_BENCH_ITERATIONS STREQUAL "")
  message(FATAL_ERROR "STALKER_BENCH_ITERATIONS must be set (e.g. -DSTALKER_BENCH_ITERATIONS=10)")
endif()
if(NOT DEFINED STALKER_BENCH_WARMUP_ITERATIONS OR STALKER_BENCH_WARMUP_ITERATIONS STREQUAL "")
  message(FATAL_ERROR "STALKER_BENCH_WARMUP_ITERATIONS must be set (e.g. -DSTALKER_BENCH_WARMUP_ITERATIONS=2)")
endif()
if(NOT STALKER_BENCH_ITERATIONS MATCHES "^[0-9]+$")
  message(FATAL_ERROR "STALKER_BENCH_ITERATIONS must be a positive integer")
endif()
if(STALKER_BENCH_ITERATIONS LESS 1)
  message(FATAL_ERROR "STALKER_BENCH_ITERATIONS must be >= 1")
endif()
if(STALKER_BENCH_ITERATIONS GREATER 100000)
  message(FATAL_ERROR "STALKER_BENCH_ITERATIONS excessively large (>100000)")
endif()
if(NOT STALKER_BENCH_WARMUP_ITERATIONS MATCHES "^[0-9]+$")
  message(FATAL_ERROR "STALKER_BENCH_WARMUP_ITERATIONS must be a non-negative integer")
endif()
if(STALKER_BENCH_WARMUP_ITERATIONS LESS 0)
  message(FATAL_ERROR "STALKER_BENCH_WARMUP_ITERATIONS must be >= 0")
endif()
if(STALKER_BENCH_WARMUP_ITERATIONS GREATER 100000)
  message(FATAL_ERROR "STALKER_BENCH_WARMUP_ITERATIONS excessively large (>100000)")
endif()

# make them literals for C++
set(_bench_iterations "${STALKER_BENCH_ITERATIONS}")
set(_bench_warmup     "${STALKER_BENCH_WARMUP_ITERATIONS}")

# ----- Memory Operations -----
if(NOT DEFINED STALKER_BENCH_MEMOPS OR STALKER_BENCH_MEMOPS STREQUAL "")
  message(FATAL_ERROR "STALKER_BENCH_MEMOPS must be set (e.g. -DSTALKER_BENCH_MEMOPS=copy;setvalue;swap)")
endif()
set(_seen_memops "")
foreach(mo IN LISTS STALKER_BENCH_MEMOPS)
  if(";${_seen_memops};" MATCHES ";${mo};")
    message(FATAL_ERROR "STALKER_BENCH_MEMOPS: duplicate entry '${mo}'")
  endif()
  set(_seen_memops "${_seen_memops};${mo}")
endforeach()

set(_bench_memops_csv_quoted "${STALKER_BENCH_MEMOPS}")
string(REPLACE ";" "\", \"" _bench_memops_csv_quoted "${_bench_memops_csv_quoted}")
set(_bench_memops_csv_quoted "\"${_bench_memops_csv_quoted}\"")

list(LENGTH STALKER_BENCH_MEMOPS _bench_memops_len)

# ----- Math Operations -----
if(NOT DEFINED STALKER_BENCH_MATHOPS OR STALKER_BENCH_MATHOPS STREQUAL "")
  message(FATAL_ERROR "STALKER_BENCH_MATHOPS must be set (e.g. -DSTALKER_BENCH_MATHOPS=dot;sum;axpy)")
endif()
set(_seen_mathops "")
foreach(mo IN LISTS STALKER_BENCH_MATHOPS)
  if(";${_seen_mathops};" MATCHES ";${mo};")
    message(FATAL_ERROR "STALKER_BENCH_MATHOPS: duplicate entry '${mo}'")
  endif()
  set(_seen_mathops "${_seen_mathops};${mo}")
endforeach()

set(_bench_mathops_csv_quoted "${STALKER_BENCH_MATHOPS}")
string(REPLACE ";" "\", \"" _bench_mathops_csv_quoted "${_bench_mathops_csv_quoted}")
set(_bench_mathops_csv_quoted "\"${_bench_mathops_csv_quoted}\"")

list(LENGTH STALKER_BENCH_MATHOPS _bench_mathops_len)

# ----- Matrix Operations -----
if(NOT DEFINED STALKER_BENCH_MATRIXOPS)
  set(STALKER_BENCH_MATRIXOPS "")
endif()
set(_seen_matrixops "")
foreach(mo IN LISTS STALKER_BENCH_MATRIXOPS)
  if(";${_seen_matrixops};" MATCHES ";${mo};")
    message(FATAL_ERROR "STALKER_BENCH_MATRIXOPS: duplicate entry '${mo}'")
  endif()
  set(_seen_matrixops "${_seen_matrixops};${mo}")
endforeach()

set(_bench_matrixops_csv_quoted "${STALKER_BENCH_MATRIXOPS}")
string(REPLACE ";" "\", \"" _bench_matrixops_csv_quoted "${_bench_matrixops_csv_quoted}")
set(_bench_matrixops_csv_quoted "\"${_bench_matrixops_csv_quoted}\"")

list(LENGTH STALKER_BENCH_MATRIXOPS _bench_matrixops_len)

# ----- Data Range & Seed -----
if(NOT DEFINED STALKER_BENCH_DATA_RANGE_MIN)
  set(STALKER_BENCH_DATA_RANGE_MIN 0)
endif()
if(NOT DEFINED STALKER_BENCH_DATA_RANGE_MAX)
  set(STALKER_BENCH_DATA_RANGE_MAX 100)
endif()
if(NOT DEFINED STALKER_BENCH_SEED)
  set(STALKER_BENCH_SEED 123456)
endif()
set(_bench_data_range_min "${STALKER_BENCH_DATA_RANGE_MIN}")
set(_bench_data_range_max "${STALKER_BENCH_DATA_RANGE_MAX}")
set(_bench_seed "${STALKER_BENCH_SEED}")

# ---- Per-operation compile definitions for selective compilation/gating ----
foreach(memop IN LISTS STALKER_BENCH_MEMOPS)
  if(NOT memop STREQUAL "")
    string(TOUPPER ${memop} memop_upper)
    target_compile_definitions(Benchmarks PRIVATE STALKER_BENCH_MEMOP_${memop_upper}=1)
  endif()
endforeach()
foreach(mathop IN LISTS STALKER_BENCH_MATHOPS)
  if(NOT mathop STREQUAL "")
    string(TOUPPER ${mathop} mathop_upper)
    target_compile_definitions(Benchmarks PRIVATE STALKER_BENCH_MATHOP_${mathop_upper}=1)
  endif()
endforeach()
foreach(matrixop IN LISTS STALKER_BENCH_MATRIXOPS)
  if(NOT matrixop STREQUAL "")
    string(TOUPPER ${matrixop} matrixop_upper)
    target_compile_definitions(Benchmarks PRIVATE STALKER_BENCH_MATRIXOP_${matrixop_upper}=1)
  endif()
endforeach()


# Generate header into the build dir
set(_bench_slave_thread_id "${_bench_slave_thread_id}")
set(_bench_num_threads "${_bench_num_threads}")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/BenchmarkConfig.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/BenchmarkConfig.hpp
  @ONLY
)

# Provide replacement tokens for BenchmarkConfig.hpp.in
set(_bench_slave_thread_id ${_bench_slave_thread_id} CACHE INTERNAL "Slave thread ID for benchmarks")
set(_bench_num_threads ${_bench_num_threads} CACHE INTERNAL "Benchmark/Eigen/BLAS thread count")

# Echo affinity choice for visibility
message(STATUS "Benchmark suite affinity core (slave_thread_id) = ${_bench_slave_thread_id}")
message(STATUS "Benchmark library thread count (Eigen/BLAS) = ${_bench_num_threads}")

# Make the generated header visible to Benchmarks
target_include_directories(Benchmarks PRIVATE ${CMAKE_CURRENT_BINARY_DIR})