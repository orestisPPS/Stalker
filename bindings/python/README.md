# Staλκer HPC Python Bindings

Welcome to the Python frontend for the Staλκer High-Performance Computing library. Built with NanoBind, this module natively exposes Staλκer's zero-cost SIMD and unrolled operations to standard Python through zero-copy `numpy.ndarray` operations.

---

## 1. Core Concepts & Configuration

Staλκer routes operations through runtime descriptors that dictate *how* the C++ engine treats the hardware layer. This is abstracted through the `stalker.core.ExecutionConfig` class, allowing fine-grained control directly from Python.

### `ExecutionConfig` Factories
- **`ExecutionConfig.Scalar(enable_std: bool = True)`**
  Falls back to standard scalar/loop implementations. If `enable_std` is `True`, it will use the C++ standard library for operations; otherwise, it will use custom scalar implementations.
- **`ExecutionConfig.Unrolled(factor: int)`**
  Employs compile-time loop unrolling templates. Common choices: `2`, `4`, `8`.
- **`ExecutionConfig.SIMD(...)`**
  The most powerful execution driver. Enforces hardware-level intrinsic routing.
  ```python
  import stalker.core as core
  
  simd_cfg = core.ExecutionConfig.SIMD(
      simd_type=core.SIMDType.AVX2,               # AVX2, AVX512, or None 
      unroll=2,                                   # Accumulator unrolling loop factor
      store=core.SIMDStorePolicy.Streamed,        # Cached or Streamed (non-temporal)
      prefetch=core.PrefetchHint.T0,              # HintT0, T1, T2, NTA, or None
      assume_aligned=True,                        # Bypass dynamic alignment runtime checks
      ilp=core.ILPPolicy.Interleaved              # Instruction-level parallelism strategy (Grouped/Interleaved)
  )
  ```

### SIMD Constants
- **`SIMDType`**: `None`, `AVX2`, `AVX512`
- **`SIMDStorePolicy`**: 
  - `Cached`: Uses standard `_mm_store_()`, populating CPU caches.
  - `Streamed`: Direct-to-RAM memory bypassing L1/L2 caches using `_mm_stream_()`. Demands strict memory alignment.
- **`PrefetchHint`**: Hardware memory lookup queues (`None`, `T0`, `T1`, `T2`, `NTA`).
- **`ILPPolicy`**: Pipelining mechanics for registers (`Grouped`, `Interleaved`).

---

## 2. Memory Module (`stalker.memory`)

All memory functions operate natively on C/C++ memory but are returned directly to Python as fully operative `numpy.ndarray` objects! You do *not* need to wrap these explicitly in `np.asarray()`.

### Allocators
Allocators guarantee cache-line hardware alignment necessary for deep SIMD and streamer stores.

- **`empty_aligned(size: int, alignment: int = 32, dtype: str = "float32") -> numpy.ndarray`**
  Returns an uninitialized, structurally aligned NumPy array. Supported dimensions: 16, 32, 64, or 128 bytes.
- **`zeros_aligned(size: int, alignment: int = 32, dtype: str = "float32") -> numpy.ndarray`**
  Same as `empty_aligned` but securely 0-initializes the memory frame.

*Supported Python string dtypes*: `"float32"`, `"float64"`, `"int32"`, `"uint32"`, `"int16"`, or standard bare type passes (`int`, `float`).

### Memory Operations
*All functions accept an optional `config: ExecutionConfig` parameter at the end as defined above.*

- **`copy(dest: np.ndarray, src: np.ndarray, config=Default)`**
  HPC-level memory copying from `src` to `dest`.
- **`swap(data1: np.ndarray, data2: np.ndarray, config=Default)`**
  Bi-directional inplace element swapping.
- **`set_value(dest: np.ndarray, value: scalar, config=Default)`**
  Aggressively fills an array with a targeted scalar value using SIMD propagation.

---

## 3. Vector Mathematics Module (`stalker.vector_math`)

Mathematical array-to-array transformations. Operates seamlessly on NumPy frames mapped to Stalker architectures. All computations demand equal-length arrays and bypass dynamic checks unless specified.
*All functions accept an optional `config: ExecutionConfig`.*

### In-Place & Standard Operations
- **`add(a: np.ndarray, b: np.ndarray, res: np.ndarray, config=Default)`**
  Standard vector addition: `res = a + b`
- **`add(a: np.ndarray, b: np.ndarray, res: np.ndarray, alpha: float, beta: float, config=Default)`**
  Scaled vector addition: `res = alpha * a + beta * b`
- **`subtract(a, b, res, config=Default)`** / **`subtract(a, b, res, alpha, beta, config=Default)`**
  Subtracts `b` from `a` (optionally scaled).
- **`multiply(a, b, res, config=Default)`** / **`multiply(a, b, res, alpha, beta, config=Default)`**
  Multiplies two arrays into `res`.
- **`axpy(a, b, res, alpha: float, config=Default)`**
  Highly optimized `res = alpha * a + b` BLAS construct.
- **`scale(a: np.ndarray, res: np.ndarray, alpha: float, config=Default)`**
  Scalar matrix magnification: `res = a * alpha`.
- **`addConstant(a: np.ndarray, res: np.ndarray, value: float, config=Default)`**
  Adds a fixed numerical scalar: `res = a + value`.

### Reductions
Reductions aggregate arrays into a single numeric point.
- **`sum(a: np.ndarray, config=Default) -> float`**
  Computes the high-precision accumulation of the entire array elements.
- **`dot(a: np.ndarray, b: np.ndarray, config=Default) -> float`**
  Vectorized contiguous dot product yielding algebraic scalar.

---

## Testing & Continuous Integration

Stalker comes wrapped with an intensive predator-test orchestration that runs all SIMD/Execution combinations against standard Numpy.

```bash
# Ensure packages inside the virtual environment
pip install rich numpy
python bindings/python/tests/run_tests.py
```
