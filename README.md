# STAΛKER

> High Performance Silicon Whispering Computer Mathematics

## Overview

STAΛKER is a high-performance, header-only, C++ 17 linear algebra library. It combines data-level (SIMD) and thread-level concurrency with modern template metaprogramming techniques to obtain excellent performance, especially in memory-bound data operations. It can perform similarly or significantly better than `STL`, `OpenBLAS`, and `Eigen` across a broad range of operations and sizes. Users are free to tune and experiment with almost every aspect of the performance optimizations, from compile-time, according to their needs and hardware.

## Motivation

The motivation behind STAΛKER is to navigate The Zone of low-level, high-performance computing and foundational mathematics. It aims to obtain the maximum single CPU core performance possible, using a high-level language without resorting to assembly or chip specific kernels and total madness. Great effort has been given in order to eliminate runtime dispatch and overhead and offer extended configurability, almost exclusively at compile-time. These design principles, under the correct settings, offer portable and predictable performance from expensive modern CPUs to embedded systems.

It is a personal playground and learning space that offers easy to use experimentation with HPC concepts. 

---

## Table of Contents

- [STAΛKER](#staλker)
  - [Overview](#overview)
  - [Motivation](#motivation)
  - [Table of Contents](#table-of-contents)
  - [Core Features](#core-features)
    - [Design](#design)
    - [Memory](#memory)
    - [Mathematics](#mathematics)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Cloning Guide](#cloning-guide)
    - [Configure \& Build (CMake)](#configure--build-cmake)
    - [CMake Options](#cmake-options)
      - [Build toggles](#build-toggles)
      - [Memory and unrolling](#memory-and-unrolling)
      - [SIMD](#simd)
      - [Threading](#threading)
      - [Build Profiles](#build-profiles)
      - [Example Configuration: Performance focused release build](#example-configuration-performance-focused-release-build)
    - [Installation \& Integration](#installation--integration)
    - [Option A — Installed package (recommended)](#option-a--installed-package-recommended)
    - [Option B — Subdirectory configuration](#option-b--subdirectory-configuration)
  - [Examples](#examples)
    - [Example 1: Vectorized and unrolled copy of 32-bit alignened std::vector with AVX2 instructions and streamed store.](#example-1-vectorized-and-unrolled-copy-of-32-bit-alignened-stdvector-with-avx2-instructions-and-streamed-store)
    - [Example 2: Partially unrolled dot product of with default alignment.](#example-2-partially-unrolled-dot-product-of-with-default-alignment)
    - [Example 3: Fully unrolled compile-time sum](#example-3-fully-unrolled-compile-time-sum)
  - [Performance](#performance)
  - [License](#license)
    - [Third-party Dependencies](#third-party-dependencies)

---

## Core Features

### Design

- **SIMD** (Single Instruction, Multiple Data): Direct hardware-level parallelism using 256-bit (AVX2) and 512-bit (AVX-512) vector registers. Enables concurrent arithmetic on multiple elements per instruction. Instruction set extensions, store policies (cached/streamed stores), load policies (aligned/not-aligned loads) and prefetch hint can be configured in compile-time. Supported data types: `double`, `float`, `int`, `unsigned int`, `short`.  
- **Template Metaprogramming**: Metaprogramming techniques and templating (CRTP, template specialization, `std::index_sequence`, etc.) enable zero-overhead compile-time dispatching and static polymorphism (no vtable lookups) with type safe, almost branchless code. Users can enable and configure compile-time unrolling for fully unrolled small loops and large loops with reduced check overhead and perform arithmetic operations exclusively with the compiler.
- **Hardware Concurrency**: Dual multithreading backend support for operations on contiguous memory blocks:
  - `std::thread` Cross-platform, simple but only number of threads can be configured.
  - `pthread`  Linux only backend wrapping POSIX thread. High configurability based on the exact topology of the machine. Users can enable Simultaneous Multi-Threading (SMT), set thread number and thread affinity with custom CPU pools that share resources.
    >Parallel operations due to their nature (thread creation & launching, safety checks, synchronization) increase branching and overhead. Of course compile-time evaluations cannot be performed.

### Memory
- `MemoryOperations API`: `copy`, `setValue`, `swap`:
  - Sequential and parallel variants
  - Vectorized with AVX2 and AVX512 instruction sets (with / without unroll)
  - Loops (with / without unroll)
  - Standard Template Library algorithms 

- **Allocators**: Dynamically allocate and deallocate data with custom alignment: raw pointers, `std::vector`, `std::shared_ptr`, `std::unique_ptr`


### Mathematics
All operations marked with [constexpr] can be evaluated at compile-time.
- **VectorMath**: `add`, `axpy`, `subtract`, `multiply`, `divide`, `scale`, `addConstant`, `dot`
  - Sequential and parallel variants
  - Weighted and unweighted variants [where applicable]
  - Vectorized with AVX2 and AVX512 instruction sets (with / without unroll) [except for divide]
  - Loops (with / without unroll)
- **MatrixMath**: `add`, `subtract`, `multiply`, `scale`, `addConstant`, `dot`, `matrixVectorMultiply`, `vectorMatrixMultiply`
  - Support for full, row and column major matrices and subblocks.
  - Vectorized with AVX2 and AVX512 instruction sets (with / without unroll)
  - Sequential and parallel variants
  - Weighted and unweighted variants [where applicable]
  - Loops (with / without unroll)
- **Differentiation [constexpr]**: Numerical calculation of 1st and 2nd order derivatives (Finite Difference Method). Forward, backward and central FD schemes with error order up to $\Delta x^6$.
- **Integration [constexpr]**: Numerical integration algorithms: Trapezoidal, Simpson1, Simpson2.
- **Meta Mathematics [constexpr]**: Compile-time evaluated fundamental mathematical operations: `power`, `sumOfPower`, `factorial`, `fibonacci`.
---

## Getting Started

### Prerequisites
- Modern C++ compiler supporting C++17: GCC 7+, Clang 7+, or MSVC 19.12+ (VS2017 15.5) or newer.
- CMake (3.18 or later)
- [Optional] CPU supporting SIMD instructions (AVX2, AVX512).
    ```bash
    # Check for AVX2 and FMA support (returns flags if supported)
    grep -q "avx2" /proc/cpuinfo && grep -q "fma" /proc/cpuinfo && echo "-mavx2 -mfma"

    # Check for AVX512 support (returns flags if supported)
    grep -q "avx512f" /proc/cpuinfo && grep -q "avx512dq" /proc/cpuinfo && grep -q "avx512bw" /proc/cpuinfo && echo "-mavx512f -mavx512dq -mavx512bw -mfma"
    ```
### Cloning Guide

1) Go to the installation directory:

    ```bash
    cd /path/to/your/desired/location
    ```

2) Get the source code:

    HTTPS:
    ```bash
    git clone https://github.com/orestisPPS/Stalker.git
    ```

    SSH:
    ```bash
    git clone git@github.com:orestisPPS/Stalker.git
    ```

 3) [optional] If you later add as a submodule and need nested content:
    ```bash
    git submodule update --init --recursive
    ```

### Configure & Build (CMake)

This project uses CMake as its primary build system and the recommended tool for configuration and integration.
It offers a lot of performance tuning and almost all of it can be performed from configure or compile time. Data operations are configured with templated execution traits. Their default values are set by cmake configuration but they can be overridden.

### CMake Options
The following CMake cache variables are available to configure the library:

#### Build toggles

| Variable                   | Description              | Default |
| -------------------------- | ------------------------ | ------- |
| `STALKER_BUILD_TESTS`      | Build tests target       | `OFF`   |
| `STALKER_BUILD_BENCHMARKS` | Build benchmarks target  | `OFF`   |

#### Memory and unrolling

| Variable                | Description                                              | Default |
| ----------------------- | -------------------------------------------------------- | ------- |
| `STALKER_ALIGNMENT`     | Byte boundary for aligned allocations                    | `64`    |
| `STALKER_UNROLL_FACTOR` | Loops unrolled per block                                 | `1`     |

#### SIMD

| Variable                       | Description                                                         | Default  |
| ------------------------------ | ------------------------------------------------------------------- | -------- |
| `STALKER_SIMD_ENABLE`          | Enable SIMD vectorization                                           | `ON`     |
| `STALKER_SIMD_INSTRUCTION_SET` | SIMD ISA: `auto`, `avx2`, `avx512`, `none` (auto prefers AVX2)      | `auto`   |
| `STALKER_SIMD_STORE_POLICY`    | SIMD store policy: `stream` (non-temporal) or `cache`               | `stream` |
| `STALKER_SIMD_PREFETCH_HINT`   | Prefetch hint: `HintNone`,`HintT0`, `HintT1`, `HintT2`, `HintNTA`              | `HintNone` |

#### Threading

| Variable                               | Description                                                             | Default |
| -----------------------------------    | ----------------------------------------------------------------------- | ------- |
| `STALKER_THREADING_ENABLE`             | Enable CPU parallelization                                              | `ON`    |
| `STALKER_THREADING_MAX_THREADS_ENABLE` | Use maximum hardware threads (overrides manual thread count)          | `OFF`   |
| `STALKER_THREADING_NUM_THREADS`        | Thread count (`0` = auto-detect)                                        | `0`     |
| `STALKER_THREADING_STD_ENABLE`         | Use `std::thread` backend                                               | `ON`    |
| `STALKER_THREADING_POSIX_ENABLE`       | Use `pthread` backend (Linux/POSIX only)                                | `OFF`   |
| `STALKER_THREADING_POSIX_SMT_ENABLE`   | Enable Simultaneous Multithreading (only with `pthread` backend on Linux)            | `OFF`   |

#### Build Profiles

| Variable | Description | Default |
| -------- | ----------- | ------- |
| `STALKER_BUILD_PROFILE` | Unified build profile selector: `debug`, `release`, `perf`, `relwithdebinfo`, `custom`. | `release` |
| `STALKER_BUILD_CUSTOM_FLAGS` | Semicolon list of custom compiler flags (active only when profile=`custom`). | `` |

Profile semantics (GNU/Clang):

| Profile | Flags | Notes |
| ------- | ----- | ----- |
| `debug` | `-O0 -g` | Full symbols; no optimization. |
| `release` | `-O3 -DNDEBUG` | Standard optimized build. |
| `perf` | `-O3 -DNDEBUG -ffast-math -funroll-loops -march=native -flto -fomit-frame-pointer -falign-functions=32 -falign-loops=32 -fno-math-errno` | Maximum speed; sacrifices portability & strict IEEE semantics. |
| `relwithdebinfo` | `-O2 -g -DNDEBUG` | Mirrors CMake built-in variant. |
| `custom` | (user supplied) | Use `STALKER_BUILD_CUSTOM_FLAGS`. No automatic safety or perf flags added. |

MSVC equivalents map similarly (`/Od /Zi`, `/O2 /DNDEBUG`, `perf` adds `/Ox /GL /Ot /fp:fast`).

#### Example Configuration: Performance focused release build
- Loop unrolling factor = 2
- 64-byte bound allocations (alignment = 64)
- SIMD is enabled and configured to use AVX2 instructions with aggressive (L1, L2) prefetching. Data will be streamed to memory after the operation.
- Multithreading is enabled and the thread backend uses `pthread`. Jobs will be distributed among 4 threads which belong to the same 2 physical core units since SMT is enabled.

  ```bash
  cmake -S . -B build \
  -DSTALKER_BUILD_PROFILE=release \
  -DSTALKER_ALIGNMENT=64 \
  -DSTALKER_UNROLL_FACTOR=2 \
  -DSTALKER_SIMD_ENABLE=ON \
  -DSTALKER_SIMD_INSTRUCTION_SET=avx2 \
  -DSTALKER_SIMD_PREFETCH_HINT=HintT0 \
  -DSTALKER_SIMD_STORE_POLICY=stream \
  -DSTALKER_THREADING_ENABLE=ON \
  -DSTALKER_THREADING_NUM_THREADS=4 \
  -DSTALKER_THREADING_POSIX_ENABLE=ON \
  -DSTALKER_THREADING_POSIX_SMT_ENABLE=ON \
  ```

### Installation & Integration

Stalker is header-only and there are two ways to integrate:

### Option A — Installed package (recommended)


This produces an installable CMake package so downstream projects can use `find_package(Stalker)`.

1) Configure

- Set the values of the parameters in `stalker-build.json`. Configure using the Python helper:
    ```bash
    python3 Tools/build.py configure
    ```

- CMake (equivalent):
    ```bash
    cmake -S . -B build -DSTALKER_BUILD_PROFILE=release -DSTALKER_UNROLL_FACTOR=2 -DSTALKER_SIMD_ENABLE=ON -DSTALKER_SIMD_INSTRUCTION_SET=avx2
    ```

2) Install (header-only; optional)

- Use the Python helper:
   ```bash
   python3 Tools/build.py install --install-prefix /opt/stalker
   ```

- CMake:
    ```bash
    cmake --install build --prefix /opt/stalker
    ```

3) Use in your CMake project

    ```cmake
    find_package(Stalker REQUIRED)
    target_link_libraries(my_target PRIVATE Stalker::Stalker)
    ```
    **NOTE**: If you installed to a non-standard location, point CMake to the prefix:

    ```bash
    export CMAKE_PREFIX_PATH=/opt/stalker:$CMAKE_PREFIX_PATH
    ```
    #### Multi-Variant Packaging (AVX2 and AVX512)
    Your chosen configuration at install time (SIMD, alignment, threading flags) is exported through the INTERFACE target and propagated to dependents. To package multiple variants install to separate prefixes.

    ```bash
    # Variant 1
    cmake -S . -B build-avx2 -DSTALKER_BUILD_PROFILE=release -DSTALKER_SIMD_ENABLE=ON -DSTALKER_SIMD_INSTRUCTION_SET=avx2
    cmake --install build-avx2 --prefix /opt/stalker-v1

    # Variant 2
    cmake -S . -B build-avx512 -DSTALKER_BUILD_PROFILE=release -DSTALKER_SIMD_ENABLE=ON -DSTALKER_SIMD_INSTRUCTION_SET=avx512
    cmake --install build-avx512 --prefix /opt/stalker-v2
    ```

  Consumer projects can then point to the desired variant:
  ```bash
  export CMAKE_PREFIX_PATH=/opt/stalker-v2:$CMAKE_PREFIX_PATH
  ```

### Option B — Subdirectory configuration

Vendor Stalker in your source tree as a submodule and configure it as part of your project. 

1) Add the source

    ```bash
    git submodule add https://github.com/orestisPPS/Stalker external/Stalker
    git submodule update --init --recursive
    ```

2) In your CMakeLists.txt

    ```cmake
    add_subdirectory(external/Stalker)
    target_link_libraries(my_target PRIVATE Stalker::Stalker)
    ```

3) Configure (Optional)

    Override Stalker options from your project (if desired):

    ```bash
    cmake -S . -B build -DSTALKER_SIMD_ENABLE=ON -DSTALKER_SIMD_INSTRUCTION_SET=avx2 #configure more
    ```

## Examples
This section provides some copy-pastable C++ snippets that showcase how to configure and perform some basic memory and math operations. For more info and detailed examples with buildable and executable code visit `Stalker/Examples` and `EXAMPLES.md`.

Almost all operations can be performed with one of three backends (SIMD, Unrolled, Loops/STL). Their parameters can be set as template parameters at compile time or left defaulted to the cmake configuration defined values. Every aspect of a **single thread** operation can be configured at compile-time as template parameters with `ExecutionTraits<>`.

### Example 1: Vectorized and unrolled copy of 32-bit alignened std::vector with AVX2 instructions and streamed store.

  ```cpp
  #include <Stalker/Memory/MemoryOperations.h>
  #include <Stalker/Memory/Allocators.h>
  #include <Stalker/Mathematics/Random.h>

  int main() {
      
      //Create a SIMD execution trait SIMD Instruction Set, Unroll Factor and SIMD Store Policy
      constexpr bool IsAligned = true;
      constexpr Stalker::Core::Config::T_SIMD      Instructions = Stalker::Core::Config::T_SIMD::AVX2;
      constexpr Stalker::Core::Config::T_SIMDStore StorePolicy  = Stalker::Core::Config::T_SIMDStore::Streamed;
      constexpr Stalker::Core::Config::T_PrefetchHints PrefetchHint = Stalker::Core::Config::T_PrefetchHints::HintT0;
      constexpr size_t Unroll = 2;
      using SimdTrait = Stalker::Core::ExecutionTraitSIMD<Instructions, IsAligned, StorePolicy, Unroll, PrefetchHint>;

      //Create 2 std::vector<double> aligned at 32 bits and fill source with random numbers [0, 10].
      size_t size = 20;
      constexpr size_t Alignment = 32;

      auto src =  Stalker::Memory::createAlignedVector<double, Alignment>(size);
      Stalker::Mathematics::Random::uniform<double>(size, src.data(), 0, 10);

      auto dst =  Stalker::Memory::createAlignedVector<double, Alignment>(size);

      //Call copy from MemoryOperations API
      Stalker::Memory::MemoryOperations::copy<double, SimdTrait>(size, dst.data(), src.data());

      return 0;
  }

  ```

  ### Example 2: Partially unrolled dot product of with default alignment.

  ```cpp

  #include <Stalker/Mathematics/Vector/VectorMath.h>
  #include <Stalker/Memory/Allocators.h>
  #include <Stalker/Mathematics/Random.h>

  int main() {
      
      using Trait = Stalker::Core::ExecutionTraitUnrolled<8>;

  //Create 2 raw pointers with default alignment and fill with random numbers [0, 10].
      size_t size = 20;

      auto a =  Stalker::MemorcreateAlignedPtrRaw<double>(size);
      Stalker::Mathematics::Random::uniform<double>(size, a, 0, 10);
      auto b =  Stalker::MemorcreateAlignedPtrRaw<double>(size);
      Stalker::Mathematics::Random::uniform<double>(size, b, 0, 10);

      //Call dot from VectorMath API.
      //2 unrolled blocks with size 8 will be calculated in 2 loops and the tail (4 elements) will be handled by a c-style loop.
      auto res = Stalker::Mathematics::VectorMath::dot<double, Trait>(size, a, b);

      delete a;
      delete b;

      return 0;
  }
  ```

  ### Example 3: Fully unrolled compile-time sum

  ```cpp

  #include <Stalker/Mathematics/Vector/VectorMath.h>
  #include <array>

  int main() {
      
      constexpr auto Size = 5;
      using Trait = Stalker::Core::ExecutionTraitUnrolled<Size>;

      //Create 1 std::array<double, 5> of size 5.
      constexpr auto array = std::array<double, Size> {1, 1, 1, 1, 1};

      //Call sum from VectorMath API
      //Everything is calculated by the compiler!
      constexpr auto res = Stalker::Mathematics::VectorMath::sum<double, Trait>(Size, array.data());
      
      //This will not build if the result is not 5;
      static_assert(res == 5);

      return 0;
  }
  ```

## Performance

For detailed performance results and benchmark instructions visit `Benchmarks/README.MD`.

## License

This project is licensed under the Apache License, Version 2.0. See the [LICENCE.md](LICENCE.md) file for details.

### Third-party Dependencies
The core library (`Stalker/Core`, `Memory`, `Mathematics`) is header-only and standard C++17 compliant with **no external dependencies**.

The **Benchmarks** module optionally links against:
- **Eigen** (MPL2)
- **OpenBLAS** (BSD-3-Clause)
- **PAPI** (BSD-Style)

These are only required for building the benchmark executable and are not part of the library distribution.