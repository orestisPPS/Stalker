"""Dashboard test engine — thin adapter around stalker_tests.

All test logic lives in stalker_tests/. This file just wraps
the shared evaluate functions and returns dashboard model objects.
"""
import gc
import time

from stalker_tests import generate_configurations, evaluate_memory, evaluate_math, verify_invariants, detect_available_simd
from ..models.results import TestResult, SuiteResults, SessionResults


def _dict_to_result(d: dict) -> TestResult:
    return TestResult(
        op=d["op"], status=d["status"],
        time_us=d["time_us"], py_time_us=d.get("py_time_us", 0.0),
        err=d.get("err", 0.0), msg=d.get("msg", ""),
    )


class TestEngine:
    """Orchestrates stalker Python-binding tests and returns structured SessionResults."""

    def run(self, suites=("memory", "math"), size=1_000_000,
            dtypes=None, boundaries=None, configs=None,
            simd_types=None, store_policies=None, ilp_policies=None,
            progress_callback=None) -> SessionResults:
        if dtypes is None:
            dtypes = ["float32", "float64", "int32", "uint32", "int16"]
        if boundaries is None:
            boundaries = ["Aligned (64B)", "Unaligned"]
        if configs is None:
            configs = generate_configurations(
                simd_types=simd_types,
                store_policies=store_policies,
                ilp_policies=ilp_policies,
            )

        session = SessionResults()
        suite_runners = []
        if "memory" in suites:
            suite_runners.append(("Memory", evaluate_memory))
        if "math" in suites:
            suite_runners.append(("VectorMath", evaluate_math))

        total = len(suite_runners) * len(configs) * len(dtypes) * len(boundaries)
        current = 0

        for suite_name, evaluate_fn in suite_runners:
            for c_name, config in configs:
                for dt in dtypes:
                    for bound in boundaries:
                        current += 1
                        if progress_callback:
                            progress_callback(current, total, f"{suite_name} | {c_name} | {dt} | {bound}")

                        raw_results = evaluate_fn(size, dt, config, bound, c_name)
                        results = [_dict_to_result(r) for r in raw_results]

                        if all(r.status == "INTERCEPTED" for r in results):
                            continue

                        session.suites.append(SuiteResults(
                            suite_name=suite_name, config_name=c_name,
                            dtype=dt, boundary=bound, results=results,
                        ))
                gc.collect()

        session.finished_at = time.time()
        return session
