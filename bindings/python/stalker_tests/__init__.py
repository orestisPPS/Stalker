"""
stalker_tests — Single source of truth for Stalker HPC Python binding tests.

Both the CLI runner (tests/run_tests.py) and the Streamlit dashboard
import from this package. Add new operations here ONCE.
"""
from .core import DataGenerator, KernelMeasurer, generate_configurations, detect_available_simd
from .suites import evaluate_memory, evaluate_math, verify_invariants
