"""Core test infrastructure — data generation, measurement, configuration.

This module owns the canonical definitions. Nothing here depends on rich or streamlit.
"""
import numpy as np
from time import perf_counter

import stalker

core = stalker.core
math_ops = stalker.vector_math
mem_ops = stalker.memory


# ── Data generation ──────────────────────────────────────────────────
class DataGenerator:
    @staticmethod
    def aligned(size: int, dtype: str) -> np.ndarray:
        if size == 0:
            return np.array([], dtype=dtype)
        a = mem_ops.zeros_aligned(size, 64, dtype)
        if dtype in ("float32", "float64"):
            a[:] = np.random.rand(size).astype(dtype) * 100
        else:
            a[:] = np.random.randint(0, 100, size, dtype=dtype)
        return a

    @staticmethod
    def unaligned(size: int, dtype: str) -> np.ndarray:
        raw = np.random.rand(size + 1).astype(dtype) * 100.0
        if dtype not in ("float32", "float64"):
            raw = raw.astype(dtype)
        return raw[1:]


# ── Kernel measurement ───────────────────────────────────────────────
class KernelMeasurer:
    @staticmethod
    def measure(op_name, execute_fn, expected, dtype, numpy_fn=None):
        """Execute kernel, measure timing, compute L_inf error.  Returns a plain dict."""
        py_time_us = 0.0
        if numpy_fn is not None:
            try:
                t0 = perf_counter()
                numpy_fn()
                py_time_us = (perf_counter() - t0) * 1e6
            except Exception:
                py_time_us = 0.0
        try:
            t0 = perf_counter()
            actual = execute_fn()
            time_us = (perf_counter() - t0) * 1e6

            if expected is None:
                err = 0.0
                rel_err = 0.0
            else:
                if np.isscalar(actual) or not hasattr(actual, "astype"):
                    err = float(np.abs(float(actual) - float(expected)))
                    rel_err = err / (float(np.abs(float(expected))) + 1e-8)
                else:
                    diff = np.abs(actual.astype(np.float64) - expected.astype(np.float64))
                    err = float(np.max(diff)) if diff.size > 0 else 0.0
                    if diff.size > 0:
                        rel_err = float(np.max(diff / (np.abs(expected.astype(np.float64)) + 1e-8)))
                    else:
                        rel_err = 0.0

            is_float = dtype in ("float32", "float64")
            tol = 1e-4 if dtype == "float32" else 1e-7
            passed = (rel_err <= tol * 10 or err <= tol) if is_float else (err == 0.0)

            return {
                "op": op_name,
                "status": "PASS" if passed else "FAIL",
                "time_us": time_us,
                "py_time_us": py_time_us,
                "err": rel_err,
                "msg": "" if passed else "Deviation Limit Exceeded",
            }
        except Exception as e:
            msg = str(e)
            if "strict alignment" in msg or "inherently require aligned" in msg or "AlignmentPolicy" in msg:
                return {"op": op_name, "status": "INTERCEPTED", "time_us": 0.0, "py_time_us": py_time_us, "err": 0.0, "msg": ""}
            return {"op": op_name, "status": "FAIL", "time_us": 0.0, "py_time_us": py_time_us, "err": 0.0, "msg": msg}


# ── Configuration generation ─────────────────────────────────────────

def detect_available_simd() -> list:
    """Probe which SIMD instruction sets actually work on this hardware.
    Returns a list of string names, e.g. ['AVX2'] or ['AVX2', 'AVX512']."""
    available = []
    # AVX2 is assumed present if the library was compiled with it
    try:
        dummy = np.zeros(1, dtype=np.float32)
        mem_ops.set_value(dummy, 1.0, core.ExecutionConfig.SIMD(simd_type=core.SIMDType.AVX2))
        available.append("AVX2")
    except Exception:
        pass
    try:
        dummy = np.zeros(1, dtype=np.float32)
        mem_ops.set_value(dummy, 1.0, core.ExecutionConfig.SIMD(simd_type=core.SIMDType.AVX512))
        available.append("AVX512")
    except Exception:
        pass
    return available


def generate_configurations(simd_types=None, store_policies=None, ilp_policies=None):
    """Build the canonical list of (name, ExecutionConfig) tuples.

    Optional filters narrow which SIMD combinations are generated:
      simd_types:     list of str, e.g. ["AVX2", "AVX512"]. None = all available.
      store_policies: list of str, e.g. ["Cached", "Streamed"]. None = all.
      ilp_policies:   list of str, e.g. ["Interleaved", "Grouped"]. None = all.
    """
    configs = [
        ("Scalar (Loop)", core.ExecutionConfig.Scalar(False)),
        ("Scalar (STD)", core.ExecutionConfig.Scalar(True)),
        ("Unrolled (x2)", core.ExecutionConfig.Unrolled(2)),
    ]

    hw_available = detect_available_simd()
    requested_simd = simd_types if simd_types else hw_available

    simd_map = {"AVX2": core.SIMDType.AVX2, "AVX512": core.SIMDType.AVX512}

    all_stores = [core.SIMDStorePolicy.Cached, core.SIMDStorePolicy.Streamed]
    all_ilps = [core.ILPPolicy.Interleaved, core.ILPPolicy.Grouped]
    prefetches = [getattr(core.PrefetchHint, "None"), core.PrefetchHint.T0]
    alignments = [core.AlignmentPolicy.Auto, core.AlignmentPolicy.Aligned, core.AlignmentPolicy.Unaligned]

    stores = [s for s in all_stores if store_policies is None or s.name in store_policies]
    ilps = [i for i in all_ilps if ilp_policies is None or i.name in ilp_policies]

    for simd_name in requested_simd:
        if simd_name not in hw_available:
            continue  # gracefully skip unsupported
        simd_enum = simd_map.get(simd_name)
        if simd_enum is None:
            continue
        for store in stores:
            for align in alignments:
                for p in prefetches:
                    for ilp in ilps:
                        c = core.ExecutionConfig.SIMD(
                            simd_type=simd_enum, unroll=2, store=store,
                            prefetch=p, ilp=ilp, aligned=align,
                        )
                        name = f"{simd_enum.name} | {store.name} | {align.name} | {p.name} | {ilp.name}"
                        configs.append((name, c))
    return configs
