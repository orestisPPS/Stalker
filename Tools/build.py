import argparse
import math
import json
import os
import shutil
import subprocess
import sys

# Attempt rich import; fallback to no-color minimal stubs if unavailable
try:
    from rich.console import Console
    from rich.panel import Panel
    from rich.table import Table
    from rich.theme import Theme
    _HAVE_RICH = True
except Exception:  # broad to cover missing dependency environments
    _HAVE_RICH = False

    class _NoRichConsole:
        def print(self, *args, **kwargs):
            # Strip any pseudo rich tags for safety
            out = []
            for a in args:
                if isinstance(a, str):
                    out.append(a.replace('[vg-error]', '').replace('[vg-ok]', '').replace('[vg-warn]', '').replace('[/vg-error]', '').replace('[/vg-ok]', '').replace('[/vg-warn]', ''))
                else:
                    out.append(str(a))
            print(*out)
        def rule(self, title, **_):
            print(f"===== {title} =====")
    class _NoPanel(str):
        pass
    class _NoTable:
        def __init__(self, *_, **__):
            self.rows = []
        def add_column(self, *_, **__):
            pass
        def add_row(self, *row):
            self.rows.append(row)
        def __str__(self):
            lines = ["Table:"]
            for r in self.rows:
                lines.append(" | ".join(r))
            return "\n".join(lines)

    Console = _NoRichConsole  # type: ignore
    Panel = lambda content, **_: content  # type: ignore
    Table = _NoTable  # type: ignore
    Theme = lambda *_ , **__: None  # type: ignore

# Van Gogh inspired palette: sunflower accents, ultramarine/prussian blues, warm whites, ochres, sap greens
_vg_theme = Theme({
    "vg-border": "#FFC300",
    "vg-title": "bold #FFC300",
    "vg-panel": "#F5F1E6",
    "vg-primary": "#274690",
    "vg-info": "#90CAF9",
    "vg-accent": "#FFC300",
    "vg-muted": "#6C757D",
    "vg-ok": "bold #4C9A2A",
    "vg-warn": "bold #CC7722",
    "vg-error": "bold #D1495B",
}) if _HAVE_RICH else None

console = Console(theme=_vg_theme) if _HAVE_RICH else Console()

class StalkerBuildConfigError(Exception):
    pass

def _list_to_cmake(values, cast=str):
    if not isinstance(values, list) or not values:
        return None
    try:
        return ";".join(cast(v) for v in values)
    except Exception:
        return None

def _safe_json_load(path):
    try:
        with open(path, "r") as f:
            return json.load(f)
    except Exception:
        return None

class BenchmarkConfig:
    """Loads benchmark-build.json and exposes conversion of fields to CMake args."""
    def __init__(self, root_dir):
        self.path = os.path.join(root_dir, "Benchmarks", "benchmark-build.json")
        self.raw = _safe_json_load(self.path) or {}




    def _compute_sizes(self):
        # 1) Use explicit sizes if provided
        raw = self.raw
        explicit = raw.get("sizes")
        if isinstance(explicit, list) and explicit:
            try:
                # Filter out zeros and ensure unique integers
                return sorted(list(set(int(x) for x in explicit if int(x) > 0)))
            except Exception:
                pass 

        # 2) Use size_region (HPC Scientific Sampling)
        region = raw.get("size_region")
        
        if isinstance(region, list) and len(region) == 2:
            lo, hi = int(region[0]), int(region[1])
            if lo > hi:
                lo, hi = hi, lo
            
            sizes = set()
            
            # Start p at the smallest power of 2 <= lo, or minimal safe SIMD size (64)
            p = 64 
            while p < lo:
                p <<= 1
            # Backtrack one step to catch the '1.5x' of the previous octave if it falls in range
            if p > 64: p >>= 1 

            while p <= hi:
                # A. The Base: 2^N (Standard alignment)
                # Example: 1024, 2048, 4096...
                if p >= lo:
                    sizes.add(p)
                
                # B. Intermediate points: 1.25, 1.5, 1.75 * 2^N
                # Increases resolution to detect cache cliffs better
                q1 = p + (p >> 2)       # 1.25x
                mid = p + (p >> 1)      # 1.50x
                q3 = mid + (p >> 2)     # 1.75x
                
                for val in (q1, mid, q3):
                    if lo <= val <= hi:
                        sizes.add(val)

                # C. The Scientific Anti-Alias: 2^N + CacheLine (64 Bytes)
                # We shift by exactly one Cache Line.
                # This breaks the "Set Associativity" stride without breaking SIMD alignment.
                # Note: 64 bytes = 16 floats (AVX512) or 8 doubles (AVX512)
                anti_alias = p + 64 
                if lo <= anti_alias <= hi and anti_alias != mid:
                    sizes.add(anti_alias)
                
                p <<= 1
            
            # Always ensure the strict boundaries are included (optional, but good for graphs)
            sizes.add(lo)
            sizes.add(hi)

            # Important: Filter out anything smaller than vector width if needed
            # and sort for the runner.
            return sorted([s for s in sizes if s >= lo and s <= hi])
        
        raise Exception("No valid 'sizes' or 'size_region' found.")

    def to_cmake_args(self):
        args = []
        # Synthesize sizes
        sizes_combined = self._compute_sizes()
        
        if sizes_combined:
            val = ";".join(str(x) for x in sizes_combined)
            args.append(f"-DSTALKER_BENCH_VECTOR_SIZES={val}")
        
        # Pass other fields
        raw = self.raw
        if "data_types" in raw:
            args.append(f"-DSTALKER_BENCH_TYPES={_list_to_cmake(raw['data_types'])}")
        if "unroll_factors" in raw:
            args.append(f"-DSTALKER_BENCH_UNROLLS={_list_to_cmake(raw['unroll_factors'])}")
        if "iterations" in raw:
            args.append(f"-DSTALKER_BENCH_ITERATIONS={raw['iterations']}")
        if "warmup_iterations" in raw:
            args.append(f"-DSTALKER_BENCH_WARMUP_ITERATIONS={raw['warmup_iterations']}")
        if "operations_memory" in raw:
            args.append(f"-DSTALKER_BENCH_MEMOPS={_list_to_cmake(raw['operations_memory'])}")
        if "operations_math" in raw:
            args.append(f"-DSTALKER_BENCH_MATHOPS={_list_to_cmake(raw['operations_math'])}")
        if "slave_thread_id" in raw:
            args.append(f"-DSTALKER_BENCH_SLAVE_THREAD_ID={raw['slave_thread_id']}")
        if "data_range_min" in raw:
            args.append(f"-DSTALKER_BENCH_DATA_RANGE_MIN={raw['data_range_min']}")
        if "data_range_max" in raw:
            args.append(f"-DSTALKER_BENCH_DATA_RANGE_MAX={raw['data_range_max']}")
        if "seed" in raw:
            args.append(f"-DSTALKER_BENCH_SEED={raw['seed']}")
        
        # Feature flags
        if raw.get("enable_eigen"):
            args.append("-DSTALKER_BENCH_EIGEN_ENABLE=ON")
        else:
            args.append("-DSTALKER_BENCH_EIGEN_ENABLE=OFF")
            
        if raw.get("enable_openblas"):
            args.append("-DSTALKER_BENCH_OPENBLAS_ENABLE=ON")
        else:
            args.append("-DSTALKER_BENCH_OPENBLAS_ENABLE=OFF")
            
        if raw.get("enable_papi"):
            args.append("-DSTALKER_BENCH_PAPI_ENABLE=ON")
        else:
            args.append("-DSTALKER_BENCH_PAPI_ENABLE=OFF")

        return args



class StalkerBuildConfig:
    SIMD_OPTIONS = {"avx2", "avx512", "esp-dsp", "auto", "none"}

    # Flat, ordered list for summary
    SUMMARY_FIELDS = [
        ("build_directory", "General build directory"),
        ("build_target", "Build target"),
        ("build_target_reason", "Target selection"),
        ("build_jobs", "Build build_jobs"),
        ("build_tests", "Build tests"),
        ("build_benchmarks", "Build benchmarks"),
        ("simd_enable", "Enable SIMD"),
        ("simd_default_instructions", "SIMD instruction set"),
        ("alignment", "Memory alignment"),
        ("unroll_factor", "Loop unroll factor"),
        ("threading_enable", "Enable threading"),
        ("threading_max_threads_enable", "Enable max threads"),
        ("threading_num_threads", "Number of threads"),
        ("threading_std_enable", "Enable std::thread"),
        ("threading_posix_enable", "Enable POSIX threads"),
        ("enableHyperthreading", "Enable hyperthreading"),
        ("build_profile", "Build profile"),
        # Benchmark details (if enabled)
        ("bench_vector_sizes", "Benchmark vector sizes"),
        ("bench_types", "Benchmark data types"),
        ("bench_unrolls", "Benchmark unroll factors"),
        ("bench_memops", "Benchmark memory ops"),
        ("bench_mathops", "Benchmark math ops"),
        ("bench_iterations", "Benchmark iterations"),
        ("bench_warmup_iterations", "Benchmark warmup iters"),
        ("bench_enable_eigen", "Bench Eigen enable"),
        ("bench_enable_openblas", "Bench OpenBLAS enable"),
        ("bench_enable_papi", "Bench PAPI enable"),
        ("bench_slave_thread_id", "Bench suite affinity core"),
    ]

    def __init__(self, json_path="stalker-build.json"):
        self.raw = self._load_json(json_path)
        # ensure fields are always set (default if not found)
        self._parse_config()

    def _load_json(self, path):
        try:
            with open(path, "r") as f:
                return json.load(f)
        except Exception as e:
            raise StalkerBuildConfigError(f"Error reading {path}: {e}")

    def _parseConfigArgument(self, key, default, cast_type=None):
        value = self.raw.get(key, default)
        if cast_type is not None:
            try:
                value = cast_type(value)
            except Exception as e:
                raise StalkerBuildConfigError(f"Error parsing '{key}': {e}")
        return value

    def _parse_build_config(self):
        # Build directory: must be a non-empty string, must not be an existing file, must be normalized
        self.build_directory = self._parseConfigArgument("build_directory", "build", str)
        if not self.build_directory or not isinstance(self.build_directory, str):
            raise StalkerBuildConfigError("Invalid build_directory: must be a non-empty string.")
        self.build_directory = os.path.abspath(os.path.normpath(self.build_directory))
        if os.path.isfile(self.build_directory):
            raise StalkerBuildConfigError(f"build_directory '{self.build_directory}' is a file, must be a directory.")
        # Optional: enforce that build_directory is not within the source dir if you want

        # Build build_jobs
        self.build_jobs = int(self.raw.get("build_jobs", os.cpu_count() or 4))
        if self.build_jobs <= 0 or self.build_jobs > 128:
            raise StalkerBuildConfigError(f"Invalid build_jobs count: {self.build_jobs}")

        # Build target selection (auto if not specified)
        requested_target = self.raw.get("build_target", "auto")
        # Unified build profile
        self.build_profile = str(self.raw.get("build_profile", "release")).lower()
        valid_profiles = {"debug", "release", "perf", "relwithdebinfo", "custom"}
        if self.build_profile not in valid_profiles:
            raise StalkerBuildConfigError(f"Invalid build_profile '{self.build_profile}' (valid: {', '.join(sorted(valid_profiles))})")
        if self.build_profile == "debug":
            self._cmake_build_type = "Debug"
        elif self.build_profile == "relwithdebinfo":
            self._cmake_build_type = "RelWithDebInfo"
        else:
            self._cmake_build_type = "Release"

        # Whether to build tests/benchmarks
        self.build_tests = bool(self.raw.get("build_tests", True))
        self.build_benchmarks = bool(self.raw.get("build_benchmarks", True))

        # Compute build target if requested_target is 'auto' or not provided
        if requested_target is None or str(requested_target).lower() == "auto":
            if self.build_tests and self.build_benchmarks:
                self.build_target = "all"
                self.build_target_reason = "auto: tests+benchmarks → all"
            elif self.build_tests:
                self.build_target = "Tests"
                self.build_target_reason = "auto: tests only → Tests"
            elif self.build_benchmarks:
                self.build_target = "Benchmarks"
                self.build_target_reason = "auto: benchmarks only → Benchmarks"
            else:
                # Default to 'all' to build core libraries
                self.build_target = "all"
                self.build_target_reason = "auto: neither tests nor benchmarks → all (core)"
        else:
            self.build_target = str(requested_target)
            self.build_target_reason = f"requested: '{self.build_target}' from JSON"

    def _parse_simd(self):
        self.simd_enable = self._parseConfigArgument("simd_enable", False, bool)
        self.simd_default_instructions = self._parseConfigArgument("simd_default_instructions", "auto", str).lower()
        if self.simd_default_instructions not in self.SIMD_OPTIONS:
            raise StalkerBuildConfigError(f"Invalid simd_default_instructions '{self.simd_default_instructions}'. Valid: {', '.join(self.SIMD_OPTIONS)}")

    def _parse_alignment(self):
        # Only allow positive, power-of-2 values. If -1 (not set), leave unset.
        self.alignment = int(self.raw.get("alignment", -1))
        if self.alignment != -1:
            if self.alignment <= 0 or (self.alignment & (self.alignment - 1)) != 0:
                raise StalkerBuildConfigError(f"Error: alignment must be a positive power of 2 (got {self.alignment})")

    def _parse_unroll(self):
        self.unroll_factor = int(self.raw.get("unroll_factor", 1))
        if self.unroll_factor <= 0:
            raise StalkerBuildConfigError(f"Error: unroll_factor must be a positive integer (got {self.unroll_factor})")
        # Advisory warning for unusually large unrolls that may harm I-cache / register pressure
        if self.unroll_factor > 64:
            console.print(f"[vg-warn]unroll_factor {self.unroll_factor} is very large; may reduce performance due to code bloat.[/vg-warn]")

    def _parse_threading(self):
        self.threading_enable = bool(self.raw.get("threading_enable", False))
        self.threading_max_threads_enable = bool(self.raw.get("threading_max_threads_enable", False))
        self.threading_num_threads = int(self.raw.get("threading_num_threads", 0))


        self.threading_std_enable = bool(self.raw.get("threading_std_enable", False))
        self.threading_posix_enable = bool(self.raw.get("threading_posix_enable", False))
        self.enableHyperthreading = bool(self.raw.get("enableHyperthreading", False))


        # Safety checks for threading options
        if self.threading_std_enable and self.threading_posix_enable:
            raise StalkerBuildConfigError("Cannot enable both std::thread and POSIX threads at the same time.")
        if self.enableHyperthreading and self.threading_std_enable:
            self.enableHyperthreading = False  # silently ignore
        if self.enableHyperthreading and not self.threading_posix_enable:
            self.enableHyperthreading = False  # only allowed with POSIX

    def _parse_config(self):
        self._parse_build_config()
        self._parse_simd()
        self._parse_alignment()
        self._parse_unroll()
        self._parse_threading()
        self._collect_cmake_args()


    def _collect_cmake_args(self):
        self._cmake_args = [f"-DCMAKE_BUILD_TYPE={self._cmake_build_type}"]
        self._collect_project_args()
        self._collect_benchmark_args()

    def _collect_project_args(self):
        self._cmake_args.append(f"-DSTALKER_BUILD_TESTS={'ON' if self.build_tests else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_BUILD_BENCHMARKS={'ON' if self.build_benchmarks else 'OFF'}")
        if self.simd_enable:
            self._cmake_args.append("-DSTALKER_SIMD_ENABLE=ON" if self.simd_default_instructions != 'none' else "-DSTALKER_SIMD_ENABLE=OFF")
            self._cmake_args.append(f"-DSTALKER_SIMD_INSTRUCTION_SET={self.simd_default_instructions}")
        else:
            self._cmake_args.append("-DSTALKER_SIMD_ENABLE=OFF")
        self._cmake_args.append(f"-DSTALKER_ALIGNMENT={self.alignment}")
        self._cmake_args.append(f"-DSTALKER_UNROLL_FACTOR={self.unroll_factor}")
        self._cmake_args.append(f"-DSTALKER_THREADING_ENABLE={'ON' if self.threading_enable else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_MAX_THREADS_ENABLE={'ON' if self.threading_max_threads_enable else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_NUM_THREADS={self.threading_num_threads if self.threading_num_threads else 0}")
        self._cmake_args.append(f"-DSTALKER_THREADING_STD_ENABLE={'ON' if self.threading_std_enable else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_POSIX_ENABLE={'ON' if self.threading_posix_enable else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_POSIX_SMT_ENABLE={'ON' if self.enableHyperthreading else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_BUILD_PROFILE={self.build_profile}")
        custom_flags = self.raw.get("build_custom_flags")
        if self.build_profile == "custom" and isinstance(custom_flags, list) and custom_flags:
            joined = ";".join(str(f) for f in custom_flags if f)
            if joined:
                self._cmake_args.append(f"-DSTALKER_BUILD_CUSTOM_FLAGS={joined}")
        papi_dir = self.raw.get("papiDir")
        if papi_dir:
            self._cmake_args.append(f"-DCMAKE_PREFIX_PATH={papi_dir}")

    def _collect_benchmark_args(self):
        if not self.build_benchmarks:
            return
        # Load benchmark configuration from project root (parent of Tools directory)
        project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
        bench_loader = BenchmarkConfig(project_root)
        # Precompute sizes for summary
        try:
            sizes_combined = bench_loader._compute_sizes()
        except Exception:
            sizes_combined = []
        bench_cfg = bench_loader.to_cmake_args()
        if not bench_cfg:
            console.print(f"[vg-warn]No benchmark args generated. Missing or empty JSON at {bench_loader.path}[/vg-warn]")
        else:
            self._cmake_args.extend(bench_cfg)
        # Store summary fields for display
        raw = bench_loader.raw or {}
        # Prefer showing computed combined sizes; fallback to legacy keys
        self.bench_vector_sizes = ";".join(str(v) for v in (sizes_combined or raw.get("sizes") or raw.get("vector_sizes") or [])) or "N/A"
        self.bench_types = ";".join(raw.get("data_types", [])) or "N/A"
        self.bench_unrolls = ";".join(str(v) for v in raw.get("unroll_factors", [])) or "N/A"
        self.bench_memops = ";".join(raw.get("operations_memory", [])) or "N/A"
        self.bench_mathops = ";".join(raw.get("operations_math", [])) or "N/A"
        self.bench_iterations = raw.get("iterations", "N/A")
        self.bench_warmup_iterations = raw.get("warmup_iterations", "N/A")
        self.bench_enable_eigen = raw.get("enable_eigen", False)
        self.bench_enable_openblas = raw.get("enable_openblas", False)
        self.bench_enable_papi = raw.get("enable_papi", False)
        self.bench_slave_thread_id = raw.get("slave_thread_id", "N/A")

    def as_cmake_args(self):
        return list(self._cmake_args)

    def get_flat_summary(self):
        rows = []
        for field, _ in self.SUMMARY_FIELDS:
            val = getattr(self, field, "N/A")
            rows.append((field, str(val)))
        return rows

class StalkerBuilder:
    def __init__(self, config: StalkerBuildConfig):
        self.cfg = config

    def clean_build_dir(self):
        console.rule("Clean Build Directory", style="vg-border")
        if os.path.exists(self.cfg.build_directory):
            console.print(f"Removing build directory: [vg-warn]{self.cfg.build_directory}[/vg-warn]")
            shutil.rmtree(self.cfg.build_directory)
            console.print("[vg-ok]Build directory removed.[/vg-ok]")
        else:
            console.print("[vg-warn]Build directory does not exist, nothing to clean.[/vg-warn]")

    def run_setup(self):
        cmake_cmd = [
            "cmake", "-S", ".", "-B", self.cfg.build_directory
        ] + self.cfg.as_cmake_args()
        if getattr(self.cfg, "verbose", False):
            cmake_cmd.append("-DCMAKE_VERBOSE_MAKEFILE=ON")
        console.rule("CMake Configure (Setup)", style="vg-border")
        console.print(Panel(" ".join(cmake_cmd), title="CMake Command", border_style="vg-border", style="vg-panel"))
        try:
            res = subprocess.run(cmake_cmd, check=True)
        except subprocess.CalledProcessError as e:
            console.print(f"[vg-error]CMake configure failed with exit code {e.returncode}[/vg-error]")
            raise
        console.print("[vg-ok]CMake configure complete.[/vg-ok]")

    def run_build(self):
        build_cmd = [
            "cmake",
            "--build", self.cfg.build_directory,
            "--target", self.cfg.build_target,
            "-j", str(self.cfg.build_jobs)
        ]
        if getattr(self.cfg, "verbose", False):
            build_cmd.append("--verbose")
        console.rule("CMake Build", style="vg-border")
        console.print(Panel(" ".join(build_cmd), title="Build Command", border_style="vg-border", style="vg-panel"))
        try:
            res = subprocess.run(build_cmd, check=True)
        except subprocess.CalledProcessError as e:
            console.print(f"[vg-error]CMake build failed with exit code {e.returncode}[/vg-error]")
            raise
        console.print("[vg-ok]Build complete.[/vg-ok]")

    def print_summary(self):
        table = Table(title="Stalker Build Configuration", show_lines=True, title_style="vg-title")
        table.add_column("Setting", justify="right", style="vg-accent")
        table.add_column("Value", style="vg-info")
        for k, v in self.cfg.get_flat_summary():
            table.add_row(k, v)
        console.print(Panel(table, title="Build Summary", border_style="vg-border", style="vg-panel"))

def print_help():
    help_text = (
        "[vg-title]Stalker Build System[/vg-title]\n\n"
        "This script configures and builds the CMake-based Stalker project.\n"
        "It reads settings from [vg-info]stalker-build.json[/vg-info] and translates them into CMake options.\n\n"
        "[bold]Primary commands[/bold]:\n"
        "  • [vg-accent]build[/vg-accent]        Build only (requires previous configure in the build directory).\n"
        "  • [vg-accent]clean[/vg-accent]        Delete the entire build directory (no configure/build run).\n"
        "  • [vg-accent]clean-build[/vg-accent]  Clean, then fresh configure + build.\n"
        "  • [vg-accent]full[/vg-accent]         Configure + build (without deleting the build directory first).\n"
        "  • [vg-accent]help[/vg-accent]         Show this help.\n\n"
        "[bold]Example Usage for a clean build:[/bold]:\n"
        "  python3 Tools/build.py [vg-accent]clean[/vg-accent]\n\n"
        "[bold]Verbose mode[/bold]:\n"
        "  Add [vg-accent]--verbose[/vg-accent] (or [vg-accent]-v[/vg-accent]) to increase output detail.\n"
        "  This enables [vg-info]CMAKE_VERBOSE_MAKEFILE[/vg-info] during configure and passes [vg-info]--verbose[/vg-info] to the build step.\n\n"
        "[bold]Configuration via JSON[/bold] ([cyan]stalker-build.json[/cyan]):\n"
        "  • [vg-info]build_directory[/vg-info]           Target build folder (default: build).\n"
        "  • [vg-info]build_target[/vg-info]              Target name or 'auto' (default). Auto builds: both→all, tests→Tests, benchmarks→Benchmarks, none→all.\n"
        "  • [vg-info]build_jobs[/vg-info]                Parallel build jobs (integer).\n"
        "  • [vg-info]build_profile[/vg-info]             debug | release | perf | relwithdebinfo | custom.\n"
        "  • [vg-info]build_tests[/vg-info]               true/false → -DSTALKER_BUILD_TESTS.\n"
        "  • [vg-info]build_benchmarks[/vg-info]          true/false → -DSTALKER_BUILD_BENCHMARKS.\n"
        "  • [vg-info]alignment[/vg-info]                 Positive power-of-two (e.g., 64).\n"
        "  • [vg-info]simd_enable[/vg-info]               true/false → enable SIMD at configure time.\n"
        "  • [vg-info]simd_default_instructions[/vg-info] avx2 | avx512 | esp-dsp | auto | none.\n"
        "  • [vg-info]unroll_factor[/vg-info]             Positive power-of-two (1 = disabled).\n"
        "  • [vg-info]threading_enable[/vg-info]          true/false.\n"
        "  • [vg-info]threading_num_threads[/vg-info]     Thread count (0 = auto).\n"
        "  • [vg-info]threading_std_enable[/vg-info]      true/false → std::thread backend.\n"
        "  • [vg-info]threading_posix_enable[/vg-info]    true/false → pthread backend.\n"
        "  • [vg-info]threading_posix_smt_enable[/vg-info] true/false → enable SMT with pthread backend.\n"
        "  • [vg-info]papiDir[/vg-info]                    Optional path to PAPI install (for find_package).\n\n"
    )
    console.print(Panel(help_text, title="Stalker Builder Help", border_style="vg-border", style="vg-panel"))

def main():
    parser = argparse.ArgumentParser(
        description="Stalker Build Script (with rich output)",
        add_help=False
    )
    parser.add_argument(
        "step",
        choices=["configure", "build", "clean", "clean-build", "full", "install", "help"],
        nargs="?",
        default="full",
        help="Which step to run: configure, build, clean, clean-build, full, install, help (default: full)"
    )
    parser.add_argument(
        "--install-prefix",
        dest="install_prefix",
        default=None,
        help="Installation prefix (overrides CMAKE_INSTALL_PREFIX for 'install' step)."
    )
    parser.add_argument(
        "--verbose", "-v", action="store_true",
        help="Enable verbose output for CMake and build"
    )
    parser.add_argument(
        "--force-build", action="store_true",
        help="Force running the build even if configuration is header-only (tests & benchmarks both off)."
    )
    # Explicit help flag (since add_help=False)
    parser.add_argument(
        "-h", "--help",
        dest="show_help",
        action="store_true",
        help="Show this help and exit"
    )
    args = parser.parse_args()

    if args.step == "help" or getattr(args, "show_help", False):
        print_help()
        sys.exit(0)

    try:
        config = StalkerBuildConfig()
        config.verbose = args.verbose  # inject
        config.install_prefix = args.install_prefix
    except StalkerBuildConfigError as e:
        console.print(Panel(f"[vg-error]{str(e)}[/vg-error]", title="Configuration Error", border_style="vg-border", style="vg-panel"))
        sys.exit(2)

    builder = StalkerBuilder(config)
    builder.print_summary()

    try:
        if args.step == "clean":
            builder.clean_build_dir()
        elif args.step == "configure":
            builder.run_setup()
        elif args.step == "build":
            # Always attempt build when explicitly requested
            builder.run_build()
        elif args.step == "clean-build":
            builder.clean_build_dir()
            builder.run_setup()
            # Skip build if header-only configuration and not forced
            if (not config.build_tests and not config.build_benchmarks) and not args.force_build:
                console.rule("Skip Build (Header-only)", style="vg-border")
                console.print(Panel("No tests or benchmarks enabled: header-only targets produce no binaries. Skipping build step.", title="Info", border_style="vg-border", style="vg-panel"))
            else:
                builder.run_build()
        elif args.step == "full":
            builder.run_setup()
            if (not config.build_tests and not config.build_benchmarks) and not args.force_build:
                console.rule("Skip Build (Header-only)", style="vg-border")
                console.print(Panel("No tests or benchmarks enabled: header-only targets produce no binaries. Skipping build step.", title="Info", border_style="vg-border", style="vg-panel"))
            else:
                builder.run_build()
        elif args.step == "install":
            # Ensure configured first
            if not os.path.exists(os.path.join(config.build_directory, 'CMakeCache.txt')):
                builder.run_setup()
            # Perform install
            console.rule("CMake Install", style="vg-border")
            install_cmd = [
                "cmake", "--install", config.build_directory
            ]
            if config.install_prefix:
                install_cmd.extend(["--prefix", config.install_prefix])
            console.print(Panel(" ".join(install_cmd), title="Install Command", border_style="vg-border", style="vg-panel"))
            try:
                subprocess.run(install_cmd, check=True)
            except subprocess.CalledProcessError as e:
                console.print(f"[vg-error]CMake install failed with exit code {e.returncode}[/vg-error]")
                sys.exit(4)
            console.print("[vg-ok]Install complete.[/vg-ok]")
    except Exception as e:
        console.print(Panel(f"[vg-error]Build step failed: {e}[/vg-error]", title="Build Error", border_style="vg-border", style="vg-panel"))
        sys.exit(3)

if __name__ == "__main__":
    main()
