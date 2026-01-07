# STAΛKER BENCHMARKS

## Overview

This suite evaluates STAΛKER's performance against established libraries (Eigen, OpenBLAS, STL) using the PAPI (Performance Application Programming Interface) framework in a fair, accurate and reproducable way. It measures throughput, and hardware efficiency metrics across various data sizes and configurable parameters.

## Methodology

All benchmark parameters are defined in `benchmark-build.json`.

1.  **Configuration & Parameterization**:

    * **Vector/Matrix Sizes**:
      *   **Automatic Sampling (`size_region_matrix`, `size_region_vector`)**: Generates a logarithmic sequence within `[min, max]` including powers of two ($2^N$), intermediate octaves, and anti-aliased offsets ($2^N + \delta$) to expose cache associativity conflicts.
      *   **Explicit Definition (`sizes_*`)**: Overrides sampling with a manual list of sizes for targeted profiling.

      *   **`available_ram`**: Defines the physical memory ceiling to clamp automatic sampling, though explicit sizes (`sizes_*`) bypass this safety limit.

    *   **Operations**: Benchmarks cover Memory (Copy, SetValue), Vector (Dot, Axpy, Scale, Sum), and Matrix (Gemv) operations.
    
    *   **SIMD Parameters**: Multiple loop unrolling factors and cached/streamed stores can be tested via `unroll_factors` and `store_policies`. 

2.  **Environment Isolation**: The benchmark thread's affinity is pinned to a specific CPU core (configurable via `slave_thread_id`) to eliminate OS scheduler migration noise.

3.  **Execution & Measurement**: A single benchmark test is defined by a unique tuple of (Data Type, Size, Unroll Factor, SIMD Store Type).
    *   **Warmup & Sampling**: Each unique configuration tuple is executed `warmup_iterations` before measurement to prime instruction/data caches and train branch predictors plus `iterations` times with measurements to ensure statistical stability.
    
    *   **Performance Metrics**:
        *   **Time**: High-resolution wall-clock time using `Stalker::Utility::Timer`, which wraps `std::chrono::high_resolution_clock` (or `steady_clock`) to ensure monotonic timing.
        *   **PAPI Counters**: Hardware performance counters:
            *   **Cycles**
            *   **Instructions**
            *   **L2 Cache Misses**
            *   **Floating Point Operations**
        *   **Derived Metrics**:
            *   **Computational Throughput ($P$)**:
                $$ P_{\text{GFLOPS}} = \frac{N_{\text{FLOPs}}}{T_{\text{wall}} \times 10^9} $$
            *   **Memory Bandwidth ($BW$)**:
                $$ BW_{\text{GB/s}} = \frac{N_{\text{Bytes}}}{T_{\text{wall}} \times 10^9} $$
            *   **Instructions Per Cycle (IPC)**:
                $$ \text{IPC} = \frac{N_{\text{Instr}}}{N_{\text{Cycles}}} $$
            *   **Arithmetic Intensity ($AI$)**:
                $$ AI = \frac{N_{\text{FLOPs}}}{N_{\text{Bytes}}} $$
            *   **Miss Per Kilo Instructions (MPKI)**:
                $$ \text{MPKI}_{\text{L2}} = \frac{N_{\text{L2 Misses}}}{N_{\text{Instr}}} \times 1000 $$
            *   **Cache Misses Per Element (CMPE)**:
                $$ \text{CMPE}_{\text{L2}} = \frac{N_{\text{L2 Misses}}}{N_{\text{Elements}}} $$
   
    *   **Baselines**: STAΛKER is compared against highly optimized baselines:
        *   **OpenBLAS**: Using the CBLAS interface.
        *   **Eigen**: Using the Map interface with no-alias assignments.
        *   **STL**: Standard C++ library algorithms.

4.  **Data Analysis & Visualization**:
    *   **JSON Logs**: Raw results are exported to JSON files.
    *   **Python Plotter (`Tools/jlogs.py`)**: Processes logs to generate aggregated CSV statistics (`ALL_runs_summary_stats.csv`), comparative bar charts with error bars for all measurements of one size, and performance scaling visualizations across data sizes.


## Build & Execution Guide

The benchmarking workflow is automated via Python scripts to ensure environment consistency and reproducibility.

1.  **Environment Setup**:
    Initialize a Python virtual environment.
    ```bash
    python3 -m venv venv
    source venv/bin/activate
    pip install rich matplotlib numpy pandas
    ```

2.  **Compilation**:
    The build process involves configuring the library and the benchmark suite, followed by the compilation step.

    1.  **Configuration**:
        *   **Core Library (`stalker-build.json`)**: Configure build targets and hardware optimizations. For now, all benchmarks are single-threaded so threading must be disabled.
        
            ```json
            {
                "build_directory": "build",
                "build_profile": "release",
                "build_jobs": 16,
                "build_benchmarks": true,
                "simd_enable": true,
                "simd_default_instructions": "avx512",
                "threading_enable": false
            }
            ```
        *   **Benchmark Suite (`Benchmarks/benchmark-build.json`)**: Define experimental parameters.
        
            ```json
            {
                // Experimental Control
                "iterations": 20,              // Statistical stability: N samples per data point
                "warmup_iterations": 2,        // Cache priming: Eliminates cold-start artifacts
                "slave_thread_id": 0,          // Core affinity: Pins thread to specific core to avoid context switching
                "seed": 0,                     // Determinism: Fixed seed for reproducible data generation

                // Variable Space
                "data_types": ["double"],      // Precision: 'float' (32-bit) or 'double' (64-bit)
                "unroll_factors": [1, 4],      // List of unroll factors
                "store_policies": ["stream", "cache"],   // Memory Semantics: "cache" (temporal) vs "stream" (non-temporal/NT)

                // Problem Domain
                "size_region_vector": [256, 2e9],   // Logarithmic sampling range for vector ops
                "size_region_matrix": [16, 100000], // Logarithmic sampling range for matrix ops
                "available_ram": "64GB",            // Resource constraint: Caps max problem size
                
                // Operations
                "operations_vector" : ["axpy", "dot", "scale", "sum"],  // Vector math kernels
                "operations_matrix" : ["gemv"],                         // Matrix math kernels
                "operations_memory" : ["copy", "setValue", "setZero"],  // Vector memory kernels

                // Baselines
                "enable_eigen": true,          // Comparative baseline: Eigen3
                "enable_openblas": true,       // Comparative baseline: OpenBLAS
                "enable_papi": true            // Instrumentation: Hardware counter collection
            }
            ```

    1.  **Build**:
        Execute the build script to generate the benchmark binary.
        ```bash
        python3 Tools/build.py clean-build
        ```

1.  **System Preparation**:
    To ensure deterministic execution and accurate hardware counter collection, the operating system environment must be configured to minimize noise and allow low-level access.

    *   **Enable Hardware Counters (PAPI)**:
        Lower the kernel `perf_event_paranoid` level to grant userspace access to CPU performance monitoring units (PMU).
        ```bash
        sudo sysctl -w kernel.perf_event_paranoid=1
        ```

    *   **Stabilize Clock Frequency**:
        Disable dynamic frequency scaling (DVFS) and power-saving C-states on the benchmark core (Core 0) to prevent clock jitter and thermal throttling artifacts.
        ```bash
        sudo cpupower frequency-set -c 0 -g performance
        ```

2.  **Execution**:
    Run the benchmark binary.
    ```bash
    ./build/Benchmarks/Benchmarks
    ```

3.  **Analysis**:
    Process the generated JSON logs to produce statistical reports and visualization plots.
    ```bash
    python3 Tools/jlogs.py --in Benchmarks/logs/ --out your_output_directory/
    ```
  

## Hardware Topology

The benchmarks are executed on the following hardware configuration:

| Property | Value |
| :--- | :--- |
| **CPU Model** | AMD Ryzen 9 7950X 16-Core Processor |
| **Architecture** | x86_64 (Zen 4) |
| **Cores / Threads** | 16 Cores / 32 Threads |
| **Frequency** | Base: 4.5 GHz / Boost: ~5.88 GHz |
| **L1 Cache** | 32KB Data + 32KB Inst per core (1MB Total) |
| **L2 Cache** | 1MB per core (16MB Total) |
| **L3 Cache** | 32MB per CCD (64MB Total) |
| **SIMD Extensions** | AVX, AVX2, AVX512 (F, DQ, BW, VL, IFMA, VBMI, etc.), FMA3 |
| **Byte Order** | Little Endian |
| **Memory** | 62GB Total System Memory (6000MT/s DDR5)|

![Hardware Topology](data/topology_hal10000.png)

## Software Environment

| Component | Version / Details |
| :--- | :--- |
| **OS** | Ubuntu 24.04.3 LTS (Noble Numbat) |
| **Kernel** | 6.14.0-36-generic x86_64 |
| **Compiler** | GCC 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04) |
| **Build System** | CMake 3.28.3 |
| **Libraries** | PAPI, OpenBLAS, Eigen3 |


## Results

### 1. Axpy (Vector Addition with Scaling)

| Property | Value |
| :--- | :--- |
| **Operation** | $r \leftarrow \alpha X + Y$ |
| **Precision** | Double (64-bit) |
| **Unroll Factors** | 1, 4 |
| **Store Policies** | Cache (Temporal), Stream (Non-Temporal) |

<table border="0" width="100%">
  <tr>
    <td align="center">
      <img src="data/results/axpy/scaling/axpy_GB_s_double_LogLin_scaling.png" width="100%" />
      <br /><b>(a) Memory Bandwidth (GB/s)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/axpy/scaling/axpy_GFLOPS_double_LogLin_scaling.png" width="100%" />
      <br /><b>(b) Computational Throughput (GFLOPS)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/axpy/scaling/axpy_MPKI_double_LogLin_scaling.png" width="100%" />
      <br /><b>(c) L2 Cache Misses per Kilo Instructions (MPKI)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/axpy/scaling/axpy_IPC_double_LogLin_scaling.png" width="100%" />
      <br /><b>(d) Instructions Per Cycle (IPC)</b>
    </td>
  </tr>
</table>

### 2. Copy

| Property | Value |
| :--- | :--- |
| **Operation** | $Y \leftarrow X$ |
| **Precision** | Double (64-bit) |
| **Unroll Factors** | 1, 4 |
| **Store Policies** | Cache (Temporal), Stream (Non-Temporal) |

<table border="0" width="100%">
  <tr>
    <td align="center">
      <img src="data/results/copy/scaling/copy_GB_s_double_LogLin_scaling.png" width="100%" />
      <br /><b>(a) Memory Bandwidth (GB/s)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/copy/scaling/copy_MPKI_double_LogLin_scaling.png" width="100%" />
      <br /><b>(b) L2 Cache Misses per Kilo Instructions (MPKI)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/copy/scaling/copy_IPC_double_LogLin_scaling.png" width="100%" />
      <br /><b>(c) Instructions Per Cycle (IPC)</b>
    </td>
  </tr>
</table>

### 3. Scale

| Property | Value |
| :--- | :--- |
| **Operation** | $Y \leftarrow \alpha X$ |
| **Precision** | Double (64-bit) |
| **Unroll Factors** | 1, 4 |
| **Store Policies** | Cache (Temporal), Stream (Non-Temporal) |

<table border="0" width="100%">
  <tr>
    <td align="center">
      <img src="data/results/scale/scaling/scale_GB_s_double_LogLin_scaling.png" width="100%" />
      <br /><b>(a) Memory Bandwidth (GB/s)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/scale/scaling/scale_GFLOPS_double_LogLin_scaling.png" width="100%" />
      <br /><b>(b) Computational Throughput (GFLOPS)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/scale/scaling/scale_MPKI_double_LogLin_scaling.png" width="100%" />
      <br /><b>(c) L2 Cache Misses per Kilo Instructions (MPKI)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/scale/scaling/scale_IPC_double_LogLin_scaling.png" width="100%" />
      <br /><b>(d) Instructions Per Cycle (IPC)</b>
    </td>
  </tr>
</table>

### 4. Dot Product

| Property | Value |
| :--- | :--- |
| **Operation** | $r \leftarrow \sum x_i y_i$ |
| **Precision** | Double (64-bit) |
| **Unroll Factors** | 1, 4 |
| **Store Policies** | N/A (Reduction) |

<table border="0" width="100%">
  <tr>
    <td align="center">
      <img src="data/results/dot/scaling/dot_GB_s_double_LogLin_scaling.png" width="100%" />
      <br /><b>(a) Memory Bandwidth (GB/s)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/dot/scaling/dot_GFLOPS_double_LogLin_scaling.png" width="100%" />
      <br /><b>(b) Computational Throughput (GFLOPS)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/dot/scaling/dot_MPKI_double_LogLin_scaling.png" width="100%" />
      <br /><b>(c) L2 Cache Misses per Kilo Instructions (MPKI)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/dot/scaling/dot_IPC_double_LogLin_scaling.png" width="100%" />
      <br /><b>(d) Instructions Per Cycle (IPC)</b>
    </td>
  </tr>
</table>

### 5. Sum Reduction

| Property | Value |
| :--- | :--- |
| **Operation** | $r \leftarrow \sum x_i$ |
| **Precision** | Double (64-bit) |
| **Unroll Factors** | 1, 4 |
| **Store Policies** | N/A (Reduction) |

<table border="0" width="100%">
  <tr>
    <td align="center">
      <img src="data/results/sum/scaling/sum_GB_s_double_LogLin_scaling.png" width="100%" />
      <br /><b>(a) Memory Bandwidth (GB/s)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/sum/scaling/sum_GFLOPS_double_LogLin_scaling.png" width="100%" />
      <br /><b>(b) Computational Throughput (GFLOPS)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/sum/scaling/sum_MPKI_double_LogLin_scaling.png" width="100%" />
      <br /><b>(c) L2 Cache Misses per Kilo Instructions (MPKI)</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="data/results/sum/scaling/sum_IPC_double_LogLin_scaling.png" width="100%" />
      <br /><b>(d) Instructions Per Cycle (IPC)</b>
    </td>
  </tr>
</table>
