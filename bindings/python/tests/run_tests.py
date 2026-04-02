"""
Staλκer HPC — Python Binding Test Runner (CLI)

All test logic lives in stalker_tests/. This file is purely the rich
terminal renderer. Add new operations in stalker_tests/suites.py — they
will appear here AND in the Streamlit dashboard automatically.
"""
import sys
import gc

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

# ── Import shared test logic ─────────────────────────────────────────
from stalker_tests import generate_configurations, evaluate_memory, evaluate_math, verify_invariants

console = Console()


# ── Suite wrappers (rich rendering around shared evaluate fns) ────────
class _SuiteRunner:
    def __init__(self, name: str, description: str, evaluate_fn):
        self.name = name
        self.description = description
        self.evaluate_fn = evaluate_fn
        self.failed_configs = 0
        self.intercepted_configs = 0

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
            console=console,
        ) as progress:
            task = progress.add_task(description="Initializing...", title=f"{self.name}", total=total_runs)

            last_conf = None
            for c_name, config in configs:
                for dt in dtypes:
                    for bound in boundaries:
                        progress.update(task, description=f" | [dim]{c_name} | {dt} | {bound}[/]")

                        results = self.evaluate_fn(size, dt, config, bound, c_name)

                        statuses = [r["status"] for r in results]

                        def format_op(r):
                            if r["status"] == "FAIL":
                                return f"{r['op']}:[bold red]FAIL[/]"
                            err = r["err"]
                            if err == 0:
                                err_str = ""
                            elif err <= 1e-7:
                                err_str = f"([sky_blue3]ε:{err:.1e}[/])"
                            elif err <= 1e-4:
                                err_str = f"([light_goldenrod3]ε:{err:.1e}[/])"
                            else:
                                err_str = f"([indian_red1]ε:{err:.1e}[/])"
                            py_str = f"|py:{int(r['py_time_us'])}μs" if r.get("py_time_us", 0) > 0 else ""
                            return f"[grey84]{r['op']}[/]:[dim white]{int(r['time_us'])}μs{py_str}[/]{err_str}"

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


# ── Main runner ──────────────────────────────────────────────────────
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
            _SuiteRunner(
                "Staλκer Memory API",
                "Operations: Copy (cpy), Swap (swp), Set Value (set).\nValidates low-level memory block transfers mapping exactly to core Numpy equivalents without precision decay.",
                evaluate_memory,
            ),
            _SuiteRunner(
                "Staλκer Vector Mathematics",
                "Operations: Add, Sub, Mul, Scale, Constant Adds, Reductions (Sum, Dot), Norms (L1, LInf), Min/Max, Normalize.\nValidates arithmetic integrity allowing naturally accumulating IEEE-754 precision deviations.",
                evaluate_math,
            ),
        ]

        total_fails = 0
        total_intercepts = 0

        for suite in suites:
            suite.run(size, dtypes, boundaries, configs)
            total_fails += suite.failed_configs
            total_intercepts += suite.intercepted_configs

        # Edge cases
        invariant_results = verify_invariants()
        edge_fails = sum(1 for r in invariant_results if not r["passed"])

        inv_table = Table(title="☠️ Invariant Violation Exceptions (Predator Checks)", header_style="bold red")
        inv_table.add_column("Constraint Scenario", style="cyan")
        inv_table.add_column("Integrity", justify="center")
        inv_table.add_column("Observation", style="dim")
        for r in invariant_results:
            icon = "[bold green]PASS[/]" if r["passed"] else "[bold red]FAIL[/]"
            inv_table.add_row(r["scenario"], icon, r["observation"])
        console.print(inv_table)
        print()

        total_fails += edge_fails

        # Summary
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
