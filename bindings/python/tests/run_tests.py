import sys
import gc
import numpy as np
from time import perf_counter
from abc import ABC, abstractmethod

try:
    from rich.console import Console
    from rich.table import Table
    from rich.panel import Panel
    from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn, TimeElapsedColumn
    from rich.traceback import install
    install(show_locals=True)
except ImportError:
    print("Please install 'rich': pip install rich numpy")
    sys.exit(1)

import stalker
core = stalker.core
math_ops = stalker.vector_math
mem_ops = stalker.memory

console = Console()

class DataGenerator:
    """Manages creation of contiguous, properly-aligned and structurally misaligned buffer views."""
    @staticmethod
    def aligned(size: int, dtype: str) -> np.ndarray:
        if size == 0: return np.array([], dtype=dtype)
        a = mem_ops.zeros_aligned(size, 64, dtype)
        if dtype in ['float32', 'float64']:
            a[:] = np.random.rand(size).astype(dtype) * 100
        else:
            a[:] = np.random.randint(0, 100, size, dtype=dtype)
        return a

    @staticmethod
    def unaligned(size: int, dtype: str) -> np.ndarray:
        raw = np.random.rand(size + 1).astype(dtype) * 100.0
        if dtype not in ['float32', 'float64']:
            raw = raw.astype(dtype)
        return raw[1:]

class KernelMeasurer:
    @staticmethod
    def measure(op_name, execute_fn, expected, dtype):
        """Executes the kernel, measures timing, and computes L_inf norm error."""
        try:
            t0 = perf_counter()
            actual = execute_fn()
            time_us = (perf_counter() - t0) * 1e6
            
            if expected is None:
                err = 0.0
                rel_err = 0.0
            else:
                if np.isscalar(actual) or not hasattr(actual, 'astype'):
                    err = float(np.abs(float(actual) - float(expected)))
                    rel_err = err / (float(np.abs(float(expected))) + 1e-8)
                else:
                    diff = np.abs(actual.astype(np.float64) - expected.astype(np.float64))
                    err = float(np.max(diff)) if diff.size > 0 else 0.0
                    if diff.size > 0:
                        rel_err = float(np.max(diff / (np.abs(expected.astype(np.float64)) + 1e-8)))
                    else:
                        rel_err = 0.0

            is_float = dtype in ['float32', 'float64']
            tol = 1e-4 if dtype == 'float32' else 1e-7
            passed = (rel_err <= tol*10 or err <= tol) if is_float else (err == 0.0)
            
            return {
                "op": op_name,
                "status": "PASS" if passed else "FAIL",
                "time_us": time_us,
                "err": rel_err,
                "msg": "" if passed else "Deviation Limit Exceeded"
            }
        except Exception as e:
            msg = str(e)
            if "strict alignment" in msg or "inherently require aligned" in msg or "AlignmentPolicy" in msg:
                return {"op": op_name, "status": "INTERCEPTED", "time_us": 0.0, "err": 0.0, "msg": ""}
            return {"op": op_name, "status": "FAIL", "time_us": 0.0, "err": 0.0, "msg": msg}

class TestSuite(ABC):
    """Abstract base class for all test partitions."""
    def __init__(self, name: str, description: str = ""):
        self.name = name
        self.description = description
        self.failed_configs = 0
        self.intercepted_configs = 0

    @abstractmethod
    def evaluate(self, size: int, dtype: str, config: core.ExecutionConfig, boundary: str) -> list:
        pass

    def run(self, size: int, dtypes: list, boundaries: list, configs: list):
        total_runs = len(configs) * len(dtypes) * len(boundaries)
        
        console.print(Panel(f"[bold white]{self.name}[/]\n[dim grey84]{self.description}[/]", expand=False, border_style="grey37"))

        table = Table(title=f"🛡️  Validation ({size:,} elements)", header_style="bold grey84", show_lines=True, border_style="grey37")
        table.add_column("Configuration", style="light_steel_blue", no_wrap=True)
        table.add_column("Data Type", style="plum4")
        table.add_column("Memory Bounds", style="grey66", justify="center")
        table.add_column("Op Benchmarks (μs & Rel Err)")
        table.add_column("Status", justify="center")

        with Progress(
            SpinnerColumn(),
            TextColumn("[bold cyan]{task.fields[title]}[/]"),
            BarColumn(),
            TimeElapsedColumn(),
            TextColumn("   [progress.description]{task.description}"),
            console=console
        ) as progress:
            
            task = progress.add_task(description="Initializing...", title=f"{self.name}", total=total_runs)
            
            last_conf = None
            for c_name, config in configs:
                for dt in dtypes:
                    for bound in boundaries:
                        progress.update(task, description=f" | [dim]{c_name} | {dt} | {bound}[/]")
                        
                        results = self.evaluate(size, dt, config, bound)
                        
                        statuses = [r['status'] for r in results]
                        
                        def format_op(r):
                            if r['status'] == "FAIL": return f"{r['op']}:[bold red]FAIL[/]"
                            err = r['err']
                            if err == 0:
                                err_str = ""
                            elif err <= 1e-7:
                                err_str = f"([sky_blue3]ε:{err:.1e}[/])"
                            elif err <= 1e-4:
                                err_str = f"([light_goldenrod3]ε:{err:.1e}[/])"
                            else:
                                err_str = f"([indian_red1]ε:{err:.1e}[/])"
                            return f"[grey84]{r['op']}[/]:[dim white]{int(r['time_us'])}μs[/]{err_str}"

                        if "FAIL" in statuses:
                            diag = "[bold red]FAIL[/]"
                            ops_str = " ".join([format_op(r) for r in results])
                            self.failed_configs += 1
                        elif all(s == "INTERCEPTED" for s in statuses):
                            self.intercepted_configs += 1
                            progress.update(task, advance=1)
                            continue 
                        else:
                            diag = "[bold dark_sea_green4]PASS[/]"
                            ops_str = " ".join([format_op(r) for r in results])
                        
                        conf_display = c_name if c_name != last_conf else ""
                        last_conf = c_name
                        
                        table.add_row(conf_display, dt, bound, ops_str, diag)
                        progress.update(task, advance=1)
                gc.collect()

        console.print(table)
        print()

class MemoryTestSuite(TestSuite):
    def __init__(self):
        super().__init__(
            "Staλκer Memory API", 
            "Operations: Copy (cpy), Swap (swp), Set Value (set).\nValidates low-level memory block transfers mapping exactly to core Numpy equivalents without precision decay."
        )

    def evaluate(self, size: int, dtype: str, config: core.ExecutionConfig, boundary: str) -> list:
        gen = DataGenerator.aligned if boundary.startswith("Aligned") else DataGenerator.unaligned
        results = []
        is_float = dtype in ['float32', 'float64']

        # 1. COPY
        a, b = gen(size, dtype), gen(size, dtype)
        expected_cpy = b.copy()
        results.append(KernelMeasurer.measure("cpy", lambda: (mem_ops.copy(a, b, config), a)[1], expected_cpy, dtype))

        # 2. SWAP
        a, b = gen(size, dtype), gen(size, dtype)
        expected_swp = b.copy()
        results.append(KernelMeasurer.measure("swp", lambda: (mem_ops.swap(a, b, config), a)[1], expected_swp, dtype))

        # 3. SET
        a = gen(size, dtype)
        val = (42.5 if is_float else 42)
        expected_set = np.empty_like(a); expected_set.fill(val)
        results.append(KernelMeasurer.measure("set", lambda: (mem_ops.set_value(a, val, config), a)[1], expected_set, dtype))

        return results

class VectorMathTestSuite(TestSuite):
    def __init__(self):
        super().__init__(
            "Staλκer Vector Mathematics",
            "Operations: Add, Sub, Mul, Scale, Constant Adds, and Reductions (Sum, Dot).\nValidates arithmetic integrity allowing naturally accumulating IEEE-754 precision deviations."
        )

    def evaluate(self, size: int, dtype: str, config: core.ExecutionConfig, boundary: str) -> list:
        gen = DataGenerator.aligned if boundary.startswith("Aligned") else DataGenerator.unaligned
        results = []
        is_float = dtype in ['float32', 'float64']

        # Core Additions
        a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
        results.append(KernelMeasurer.measure("add", lambda: (math_ops.add(a, b, res, config), res)[1], a + b, dtype))

        a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
        alpha, beta = (2.0, 3.0) if is_float else (2, 3)
        results.append(KernelMeasurer.measure("asc", lambda: (math_ops.add(a, b, res, alpha, beta, config), res)[1], a * alpha + b * beta, dtype))

        # AXPY
        a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
        alpha_axpy = 2.0 if is_float else 2
        results.append(KernelMeasurer.measure("axp", lambda: (math_ops.axpy(a, b, res, alpha_axpy, config), res)[1], a * alpha_axpy + b, dtype))

        # Subtractions
        a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
        results.append(KernelMeasurer.measure("sub", lambda: (math_ops.subtract(a, b, res, config), res)[1], a - b, dtype))

        a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
        alpha, beta = (2.0, 3.0) if is_float else (2, 3)
        results.append(KernelMeasurer.measure("ssc", lambda: (math_ops.subtract(a, b, res, alpha, beta, config), res)[1], a * alpha - b * beta, dtype))

        # Multiplications
        a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
        results.append(KernelMeasurer.measure("mul", lambda: (math_ops.multiply(a, b, res, config), res)[1], a * b, dtype))

        a, b, res = gen(size, dtype), gen(size, dtype), gen(size, dtype)
        alpha, beta = (2.0, 3.0) if is_float else (2, 3)
        results.append(KernelMeasurer.measure("msc", lambda: (math_ops.multiply(a, b, res, alpha, beta, config), res)[1], (a * alpha) * (b * beta), dtype))

        # Scaling
        a, res = gen(size, dtype), gen(size, dtype)
        scalar = 2.5 if is_float else 2
        results.append(KernelMeasurer.measure("scl", lambda: (math_ops.scale(a, res, scalar, config), res)[1], a * scalar, dtype))

        a = gen(size, dtype)
        results.append(KernelMeasurer.measure("sci", lambda: (math_ops.scale(a, scalar, config), a)[1], a * scalar, dtype))

        # Constant Adds
        a, res = gen(size, dtype), gen(size, dtype)
        const_val = 5.5 if is_float else 5
        results.append(KernelMeasurer.measure("adc", lambda: (math_ops.addConstant(a, res, const_val, config), res)[1], a + const_val, dtype))

        a = gen(size, dtype)
        results.append(KernelMeasurer.measure("adi", lambda: (math_ops.addConstant(a, const_val, config), a)[1], a + const_val, dtype))

        # Reductions
        if is_float:
            a = gen(size, dtype)
            results.append(KernelMeasurer.measure("sum", lambda: math_ops.sum(a, config), np.sum(a, dtype=dtype), dtype))

            a, b = gen(size, dtype), gen(size, dtype)
            results.append(KernelMeasurer.measure("dot", lambda: math_ops.dot(a, b, config), np.dot(a, b), dtype))

        return results

def verify_invariants_and_edge_cases():
    table = Table(title="☠️ Invariant Violation Exceptions (Predator Checks)", header_style="bold red")
    table.add_column("Constraint Scenario", style="cyan")
    table.add_column("Integrity", justify="center")
    table.add_column("Observation", style="dim")

    config = core.ExecutionConfig.Default()
    passed = "[bold green]PASS[/]"
    failed = "[bold red]FAIL[/]"
    violations = 0

    try:
        a = DataGenerator.aligned(0, "float32"); b = DataGenerator.aligned(0, "float32")
        math_ops.add(a, b, a, config)
        table.add_row("Zero-Length Submisson", passed, "Valid execution without UB dumps")
    except Exception as e:
        table.add_row("Zero-Length Submisson", failed, str(e))
        violations += 1

    try:
        a = DataGenerator.aligned(100, "float32")
        a_orig = np.array(a, copy=True)
        math_ops.add(a, a, a, config)
        if np.allclose(a, a_orig * 2): table.add_row("Pointer Aliasing Safety", passed, "Destination mutated identically")
        else:
            table.add_row("Pointer Aliasing Safety", failed, "Mutation drift invalid")
            violations += 1
    except Exception as e:
        table.add_row("Pointer Aliasing Safety", failed, str(e))
        violations += 1

    try:
        a = DataGenerator.aligned(100, "float32"); b = DataGenerator.aligned(99, "float32"); c = DataGenerator.aligned(100, "float32")
        try:
            math_ops.add(a, b, c, config); table.add_row("Spatial Shape Equivalence", failed, "No threshold enforced")
            violations += 1
        except ValueError: table.add_row("Spatial Shape Equivalence", passed, "Prohibited out-of-bounds mapping")
    except Exception as e:
        table.add_row("Spatial Shape Equivalence", failed, str(e))
        violations += 1

    try:
        a = DataGenerator.aligned(100, "float32"); b = DataGenerator.aligned(100, "float32")
        a[50] = np.nan; b[75] = np.inf
        math_ops.add(a, b, a, config)
        if np.isnan(a[50]) and np.isinf(a[75]): table.add_row("IEEE-754 Arithmetic Propagation", passed, "Operands strictly propagate flags")
        else:
            table.add_row("IEEE-754 Arithmetic Propagation", failed, "Invalid vector flush")
            violations += 1
    except Exception as e:
        table.add_row("IEEE-754 Arithmetic Propagation", failed, str(e))
        violations += 1

    console.print(table)
    print()
    return violations

def generate_configurations():
    configs = []
    configs.append(("Scalar (Loop)", core.ExecutionConfig.Scalar(False)))
    configs.append(("Scalar (STD)", core.ExecutionConfig.Scalar(True)))
    configs.append(("Unrolled (x2)", core.ExecutionConfig.Unrolled(2)))

    available_simd = [getattr(core.SIMDType, "None"), core.SIMDType.AVX2]
    try:
        dummy = np.zeros(1, dtype=np.float32)
        mem_ops.set_value(dummy, 1.0, core.ExecutionConfig.SIMD(simd_type=core.SIMDType.AVX512))
        available_simd.append(core.SIMDType.AVX512)
    except Exception:
        pass

    stores = [core.SIMDStorePolicy.Cached, core.SIMDStorePolicy.Streamed]
    prefetches = [getattr(core.PrefetchHint, "None"), core.PrefetchHint.T0, core.PrefetchHint.NTA]
    ilps = [core.ILPPolicy.Interleaved, core.ILPPolicy.Grouped]
    alignments = [core.AlignmentPolicy.Auto, core.AlignmentPolicy.Aligned, core.AlignmentPolicy.Unaligned]

    for simd in available_simd:
        if simd == getattr(core.SIMDType, "None"): continue
        for store in stores:
            for p in prefetches:
                for ilp in ilps:
                    for align in alignments:
                        c = core.ExecutionConfig.SIMD(simd_type=simd, unroll=2, store=store, prefetch=p, ilp=ilp, aligned=align)
                        name = f"{simd.name} | {store.name} | {p.name} | {ilp.name} | {align.name}"
                        configs.append((name, c))
    return configs

class STALKERTestRunner:
    @staticmethod
    def execute():
        console.print(Panel("[bold cyan]STALKER HPC Test Architecture[/]\nIsolating validation domains using completely decoupled Object-Oriented partitions.", expand=False))
        print()

        configs = generate_configurations()
        dtypes = ["float32", "float64", "int32", "uint32", "int16"]
        boundaries = ["Aligned (64B)", "Unaligned"]
        size = 1_000_000 

        suites = [
            MemoryTestSuite(),
            VectorMathTestSuite()
        ]

        total_fails = 0
        total_intercepts = 0

        # Execute Suites Independently
        for suite in suites:
            suite.run(size, dtypes, boundaries, configs)
            total_fails += suite.failed_configs
            total_intercepts += suite.intercepted_configs

        # Edge Cases Base Rules
        edge_fails = verify_invariants_and_edge_cases()
        total_fails += edge_fails

        # Executive Summary Generation
        summary = Table(title="📊 Executive Validation Summary", header_style="bold black on white", show_lines=True)
        summary.add_column("Domain Module", justify="left")
        summary.add_column("Failing Faults", style="red", justify="center")
        summary.add_column("Hardware Bound Interceptions", style="cyan", justify="center")

        for suite in suites:
            summary.add_row(suite.name, str(suite.failed_configs), str(suite.intercepted_configs))

        summary.add_row("[dim]Core Edge/Invariant Rules[/]", str(edge_fails), "-")
        
        console.print(summary)
        print()

        if total_fails > 0:
            console.print(f"[bold red]❌ Execution aborted: {total_fails} configuration bounds compromised across modules.[/]")
            sys.exit(1)
        else:
            console.print("[bold green]✅ Success: Complete architecture passed testing with absolute structural integrity.[/]")


if __name__ == "__main__":
    STALKERTestRunner.execute()
