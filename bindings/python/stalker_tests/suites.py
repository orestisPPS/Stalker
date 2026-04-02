"""Test suites — the canonical evaluate functions for Memory and VectorMath.

Each function takes (size, dtype, config, boundary, config_name) and returns
a list of result dicts.  Add new operations HERE — both the CLI and
dashboard pick them up automatically.
"""
import numpy as np

from .core import DataGenerator, KernelMeasurer, core, math_ops, mem_ops


# ── Memory suite ─────────────────────────────────────────────────────
def evaluate_memory(size: int, dtype: str, config, boundary: str, config_name: str = "") -> list:
    gen = DataGenerator.aligned if boundary.startswith("Aligned") else DataGenerator.unaligned
    results = []
    is_float = dtype in ("float32", "float64")

    # COPY
    a, b = gen(size, dtype), gen(size, dtype)
    expected_cpy = b.copy()
    results.append(KernelMeasurer.measure("cpy", lambda: (mem_ops.copy(a, b, config), a)[1], expected_cpy, dtype, numpy_fn=lambda: np.copyto(a, b)))

    # SWAP
    a, b = gen(size, dtype), gen(size, dtype)
    expected_swp = b.copy()
    results.append(KernelMeasurer.measure("swp", lambda: (mem_ops.swap(a, b, config), a)[1], expected_swp, dtype, numpy_fn=lambda: (a.copy(), b.copy())))

    # SET
    a = gen(size, dtype)
    val = 42.5 if is_float else 42
    expected_set = np.empty_like(a); expected_set.fill(val)
    results.append(KernelMeasurer.measure("set", lambda: (mem_ops.set_value(a, val, config), a)[1], expected_set, dtype, numpy_fn=lambda: a.fill(val)))

    return results


# ── Vector math suite ────────────────────────────────────────────────
def evaluate_math(size: int, dtype: str, config, boundary: str, config_name: str = "") -> list:
    gen = DataGenerator.aligned if boundary.startswith("Aligned") else DataGenerator.unaligned
    results = []
    is_float = dtype in ("float32", "float64")
    is_scalar = config_name.startswith("Scalar")

    # Core Additions
    a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
    results.append(KernelMeasurer.measure("add", lambda: (math_ops.add(a, b, res, config), res)[1], a + b, dtype, numpy_fn=lambda: np.add(a, b)))

    a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
    alpha, beta = (2.0, 3.0) if is_float else (2, 3)
    results.append(KernelMeasurer.measure("add_sc", lambda: (math_ops.add(a, b, res, alpha, beta, config), res)[1], a * alpha + b * beta, dtype, numpy_fn=lambda: a * alpha + b * beta))

    # AXPY
    a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
    alpha_axpy = 2.0 if is_float else 2
    results.append(KernelMeasurer.measure("axpy", lambda: (math_ops.axpy(a, b, res, alpha_axpy, config), res)[1], a * alpha_axpy + b, dtype, numpy_fn=lambda: a * alpha_axpy + b))

    # Subtractions
    a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
    results.append(KernelMeasurer.measure("sub", lambda: (math_ops.subtract(a, b, res, config), res)[1], a - b, dtype, numpy_fn=lambda: np.subtract(a, b)))

    a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
    alpha, beta = (2.0, 3.0) if is_float else (2, 3)
    results.append(KernelMeasurer.measure("sub_sc", lambda: (math_ops.subtract(a, b, res, alpha, beta, config), res)[1], a * alpha - b * beta, dtype, numpy_fn=lambda: a * alpha - b * beta))

    # Multiplications
    a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
    results.append(KernelMeasurer.measure("mul", lambda: (math_ops.multiply(a, b, res, config), res)[1], a * b, dtype, numpy_fn=lambda: np.multiply(a, b)))

    a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
    alpha, beta = (2.0, 3.0) if is_float else (2, 3)
    results.append(KernelMeasurer.measure("mul_sc", lambda: (math_ops.multiply(a, b, res, alpha, beta, config), res)[1], (a * alpha) * (b * beta), dtype, numpy_fn=lambda: (a * alpha) * (b * beta)))

    # Scaling
    a, res = gen(size, dtype), gen(size, dtype)
    scalar = 2.5 if is_float else 2
    results.append(KernelMeasurer.measure("scl", lambda: (math_ops.scale(a, res, scalar, config), res)[1], a * scalar, dtype, numpy_fn=lambda: a * scalar))

    a = gen(size, dtype)
    results.append(KernelMeasurer.measure("scl_i", lambda: (math_ops.scale(a, scalar, config), a)[1], a * scalar, dtype, numpy_fn=lambda: a * scalar))

    # Constant Adds
    a, res = gen(size, dtype), gen(size, dtype)
    const_val = 5.5 if is_float else 5
    results.append(KernelMeasurer.measure("add_c", lambda: (math_ops.addConstant(a, res, const_val, config), res)[1], a + const_val, dtype, numpy_fn=lambda: a + const_val))

    a = gen(size, dtype)
    results.append(KernelMeasurer.measure("add_ci", lambda: (math_ops.addConstant(a, const_val, config), a)[1], a + const_val, dtype, numpy_fn=lambda: a + const_val))

    # Reductions (float-only)
    if is_float:
        a = gen(size, dtype)
        results.append(KernelMeasurer.measure("sum", lambda: math_ops.sum(a, config), np.sum(a, dtype=dtype), dtype, numpy_fn=lambda: np.sum(a, dtype=dtype)))

        a, b = gen(size, dtype), gen(size, dtype)
        results.append(KernelMeasurer.measure("dot", lambda: math_ops.dot(a, b, config), np.dot(a, b), dtype, numpy_fn=lambda: np.dot(a, b)))

    # Max / Min / Index / Norms L1 LInf (scalar-only, no config)
    if is_scalar:
        a = gen(size, dtype)
        results.append(KernelMeasurer.measure("max", lambda: math_ops.max(a), np.max(a), dtype, numpy_fn=lambda: np.max(a)))

        a = gen(size, dtype)
        results.append(KernelMeasurer.measure("min", lambda: math_ops.min(a), np.min(a), dtype, numpy_fn=lambda: np.min(a)))

        a = gen(size, dtype)
        results.append(KernelMeasurer.measure("maxIdx", lambda: math_ops.maxIndex(a), int(np.argmax(a)), dtype, numpy_fn=lambda: np.argmax(a)))

        a = gen(size, dtype)
        results.append(KernelMeasurer.measure("minIdx", lambda: math_ops.minIndex(a), int(np.argmin(a)), dtype, numpy_fn=lambda: np.argmin(a)))

        a = gen(size, dtype)
        results.append(KernelMeasurer.measure("nL1", lambda: math_ops.normL1(a), float(np.sum(np.abs(a.astype(np.float64)))), dtype, numpy_fn=lambda: np.sum(np.abs(a.astype(np.float64)))))

        a = gen(size, dtype)
        results.append(KernelMeasurer.measure("nLinf", lambda: math_ops.normLInf(a), float(np.max(np.abs(a.astype(np.float64)))), dtype, numpy_fn=lambda: np.max(np.abs(a.astype(np.float64)))))

    # Normalize (float-only, dispatched with config)
    if is_float:
        a = gen(size, dtype)
        res_norm = gen(size, dtype)
        np_norm = np.linalg.norm(a.astype(np.float64))
        expected_norm = (a.astype(np.float64) / np_norm).astype(dtype) if np_norm != 0 else a.copy()
        results.append(KernelMeasurer.measure("nrm", lambda: (math_ops.normalize(a, res_norm, config), res_norm)[1], expected_norm, dtype, numpy_fn=lambda: a / np.linalg.norm(a)))

        a_ip = gen(size, dtype)
        np_norm_ip = np.linalg.norm(a_ip.astype(np.float64))
        expected_norm_ip = (a_ip.astype(np.float64) / np_norm_ip).astype(dtype) if np_norm_ip != 0 else a_ip.copy()
        results.append(KernelMeasurer.measure("nrm_i", lambda: (math_ops.normalize(a_ip, config), a_ip)[1], expected_norm_ip, dtype, numpy_fn=lambda: a_ip / np.linalg.norm(a_ip)))

    return results


# ── Invariant / edge-case checks ─────────────────────────────────────
def verify_invariants() -> list:
    """Run edge-case checks. Returns list of dicts with keys: scenario, passed, observation."""
    config = core.ExecutionConfig.Default()
    results = []

    # Zero-Length
    try:
        a = DataGenerator.aligned(0, "float32"); b = DataGenerator.aligned(0, "float32")
        math_ops.add(a, b, a, config)
        results.append({"scenario": "Zero-Length Submission", "passed": True, "observation": "Valid execution without UB dumps"})
    except Exception as e:
        results.append({"scenario": "Zero-Length Submission", "passed": False, "observation": str(e)})

    # Pointer Aliasing
    try:
        a = DataGenerator.aligned(100, "float32")
        a_orig = np.array(a, copy=True)
        math_ops.add(a, a, a, config)
        if np.allclose(a, a_orig * 2):
            results.append({"scenario": "Pointer Aliasing Safety", "passed": True, "observation": "Destination mutated identically"})
        else:
            results.append({"scenario": "Pointer Aliasing Safety", "passed": False, "observation": "Mutation drift invalid"})
    except Exception as e:
        results.append({"scenario": "Pointer Aliasing Safety", "passed": False, "observation": str(e)})

    # Spatial Shape
    try:
        a = DataGenerator.aligned(100, "float32"); b = DataGenerator.aligned(99, "float32"); c = DataGenerator.aligned(100, "float32")
        try:
            math_ops.add(a, b, c, config)
            results.append({"scenario": "Spatial Shape Equivalence", "passed": False, "observation": "No threshold enforced"})
        except ValueError:
            results.append({"scenario": "Spatial Shape Equivalence", "passed": True, "observation": "Prohibited out-of-bounds mapping"})
    except Exception as e:
        results.append({"scenario": "Spatial Shape Equivalence", "passed": False, "observation": str(e)})

    # IEEE-754
    try:
        a = DataGenerator.aligned(100, "float32"); b = DataGenerator.aligned(100, "float32")
        a[50] = np.nan; b[75] = np.inf
        math_ops.add(a, b, a, config)
        if np.isnan(a[50]) and np.isinf(a[75]):
            results.append({"scenario": "IEEE-754 Arithmetic Propagation", "passed": True, "observation": "Operands strictly propagate flags"})
        else:
            results.append({"scenario": "IEEE-754 Arithmetic Propagation", "passed": False, "observation": "Invalid vector flush"})
    except Exception as e:
        results.append({"scenario": "IEEE-754 Arithmetic Propagation", "passed": False, "observation": str(e)})

    return results
