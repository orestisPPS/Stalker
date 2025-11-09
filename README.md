# STAΛKER

> High Performance Computer Mathematics

## Overview

STAΛKER is a high-performance, header-only, C++ 17 linear algebra library. It combines data-level (SIMD) and thread-level concurrency with modern template metaprogramming techniques to obtain excellent performance and linear scaling, especially in memory-bound data operations. It can outperform `STL`, `OpenBLAS`, and `Eigen` by up to 30% across a broad range of vector sizes and operations. Users are free to tune almost every aspect of the performance optimizations according to their needs and hardware.

The motivation behind STAΛKER is to navigate The Zone of low-level, high-performance computing and foundational mathematics. It aims to obtain the maximum single CPU core performance possible using a high-level language without resorting to assembly and chip specific kernels (and total madness). It is also a personal playground and learning space that offers easy to use experimentation with HPC concepts. 

---

## Table of Contents

- [STAΛKER](#staλker)
  - [Overview](#overview)
  - [Table of Contents](#table-of-contents)
  - [Core Features](#core-features)
    - [Design](#design)
    - [Memory](#memory)
    - [Mathematics](#mathematics)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Cloning Guide](#cloning-guide)
  - [Configuration](#configuration)
    - [Options](#options)
    - [Example](#example)
  - [Installation \& Integration](#installation--integration)
    - [Option A — Installed package (recommended)](#option-a--installed-package-recommended)
    - [Option B — Subdirectory configuration](#option-b--subdirectory-configuration)
  - [License](#license)

---

## Core Features

### Design

- **SIMD**: (Single Instruction Multiple Data) Direct hardware access via 256-bit (AVX2) and 512-bit (AVX512) registers. Concurrent operation on multiple data entries with a single CPU instruction. Users can adjust the instruction sets and the store policy (store in cache / stream in memory)
- **SIMD** (Single Instruction, Multiple Data): Direct hardware-level parallelism using 256-bit (AVX2) and 512-bit (AVX-512) vector registers. Enables concurrent arithmetic on multiple elements per instruction. Users can configure instruction sets and store policies (cached vs streaming stores).
- **Template Metaprogramming**: Metaprogramming techniques (CRTP, partial specialization, `std::index_sequence`) enable zero-overhead compile-time evaluations and polymorphism (no vtable lookups) with type safe, branchless code. Users can enable and configure compile-time unrolling for fully unrolled small loops and large loops with reduced check overhead.
- **Hardware Concurrency**: Dual multithreading backend support:
  - `std::thread` Cross-platform, simple but only number of threads can be configured.
  - `pthread`  Linux only backend wrapping posix thread. High configurability based on the exact topology of the machine. Users can enable Simultaneous Multi-Threading (SMT), set thread number and thread affinity with custom cpu pools that share resources. 

### Memory
- **Operations**: `copy`, `setValue`, `setZero`, `areEqual`:
  - Sequential and parallel variants
  - Vectorized with AVX2 and AVX512 instruction sets (with / without unroll)
  - Loops (with / without unroll)
  - Standard Template Library algorithms 

- **Allocators**: Dynamically allocate and deallocate data with custom alignment: raw pointers, `std::vector`, `std::shared_ptr`, `std::unique_ptr`


### Mathematics
All operations marked with [constexpr] can be evaluated at compile-time.
- **Operations**: `add`, `subtract`, `multiply`, `divide`, `scale`, `addConstant`, `dot`
  - Sequential and parallel variants
  - Weighted and unweighted variants
  - Vectorized with AVX2 and AVX512 instruction sets (with / without unroll) [except for divide]
  - Loops (with / without unroll)
- **Differantiation [constexpr]**: Numerical calculation of 1st and 2nd order derivatives (Finite Difference Method). Forward, backward and central FD schemes with error order up to $\Delta x^6$
- **Integration [constexpr]**: Numerical integration algorithms: Trapezoidal, Simpson1, Simpson2
- **Meta Mathematics [constexpr]**: Compile-time evaluated fundamental mathematial operations: `power`, `sumOfPower`, `factorial`, `fibonacci` 
---

## Getting Started

### Prerequisites
- Modern C++ compiler (GCC 8+, Clang 5+, MSVC 2019+) supporting C++17.
- CMake (3.10 or later)
- [Optional] CPU supporting SIMD instructions (AVX2, AVX512, esp-dsp).
    ```bash
    # Check for AVX2 and FMA support (returns flags if supported)
    grep -q "avx2" /proc/cpuinfo && grep -q "fma" /proc/cpuinfo && echo "-mavx2 -mfma"

    # Check for AVX512 support (returns flags if supported)
    grep -q "avx512f" /proc/cpuinfo && grep -q "avx512dq" /proc/cpuinfo && grep -q "avx512bw" /proc/cpuinfo && echo "-mavx512f -mavx512dq -mavx512bw -mfma"
    ```
### Cloning Guide

Go to the installation directory:

```bash
cd /path/to/your/desired/location
```

Get the source code:

HTTPS:
```bash
git clone https://github.com/orestisPPS/Stalker.git
```
SSH:
```bash
git clone git@github.com:orestisPPS/Stalker.git
```
If you later add as a submodule and need nested content:
```bash
git submodule update --init --recursive
```
---

## Configuration
### Options
The following CMake cache variables are available to configure the library: 

| Variable                             | Description                           | Default  |
| ------------------------------------ | ------------------------------------- | -------- |
| `STALKER_SIMD_ENABLE`                | Enable SIMD vectorization             | `ON`     |
| `STALKER_SIMD_INSTRUCTION_SET`       | Target instruction set (auto, avx2, avx512, esp-dsp, none) | `auto` |
| `STALKER_SIMD_STORE_POLICY`          | Memory store policy (stream/cache)    | `stream` |
| `STALKER_UNROLL_FACTOR`              | Loop unroll factor                    | `1`      |
| `STALKER_THREADING_ENABLE`           | Enable threading support              | `ON`     |
| `STALKER_THREADING_NUM_THREADS`      | Default thread count (0 = auto-detect)| `0`      |
| `STALKER_THREADING_STD_ENABLE`       | Enable std::thread backend            | `OFF`    |
| `STALKER_THREADING_POSIX_ENABLE`     | Enable pthread backend (Linux/POSIX)  | `ON`     |
| `STALKER_THREADING_POSIX_SMT_ENABLE` | Enable hyperthreading (Linux/pthreads)| `ON`     |
| `STALKER_ALIGNMENT`                  | Memory alignment (bytes)              | `64`     |
| `STALKER_ALL_GAS_NO_BRAKES`          | Aggressive optimizations              | `OFF`    |

- NOTE: The values set at configure time are set as defaults but their values can be overriden in the code.

### Example
Performance focused release build:
- Loop unrolling factor = 2
- 64-byte bound allocations (alignment = 64)
- SIMD is enabled and configured to use AVX2 instructions. Data will be streamed to memory after the operation.
- Multithreading is enabled and the thread backend uses `pthread`. Jobs will be distributed among 4 threads which belong to the same 2 physical core units since SMT is enabled.

    ```bash
    cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DSTALKER_ALIGNMENT=64 \
    -DSTALKER_UNROLL_FACTOR=2 \
    -DSTALKER_SIMD_ENABLE=ON -DSTALKER_SIMD_INSTRUCTION_SET=avx2 -DSTALKER_SIMD_STORE_POLICY=stream \
    -DSTALKER_THREADING_ENABLE=ON -DSTALKER_THREADING_NUM_THREADS=4 \
    -DSTALKER_THREADING_POSIX_ENABLE=ON -DSTALKER_THREADING_POSIX_SMT_ENABLE=ON
    ```

## Installation & Integration

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
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSTALKER_UNROLL_FACTOR=2 -DSTALKER_SIMD_ENABLE=ON -DSTALKER_SIMD_INSTRUCTION_SET=avx2
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
    cmake -S . -B build-avx2 -DCMAKE_BUILD_TYPE=Release -DSTALKER_SIMD_ENABLE=ON -DSTALKER_SIMD_INSTRUCTION_SET=avx2
    cmake --install build-avx2 --prefix /opt/stalker-v1

    # Variant 2
    cmake -S . -B build-avx512 -DCMAKE_BUILD_TYPE=Release -DSTALKER_SIMD_ENABLE=ON -DSTALKER_SIMD_INSTRUCTION_SET=avx512
    cmake --install build-avx512 --prefix /opt/stalker-v2
    ```

    Consumer projects can then point to the desired variant:
    ```bash
    export CMAKE_PREFIX_PATH=/opt/stalker-avx512:$CMAKE_PREFIX_PATH
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


## License

This library is released under the MIT License. See the `LICENSE` file for details.