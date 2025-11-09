import argparse
import json
import os
import shutil
import subprocess
import sys
from rich.console import Console
from rich.panel import Panel
from rich.table import Table
from rich.theme import Theme

# Van Gogh inspired palette: sunflower accents, ultramarine/prussian blues, warm whites, ochres, sap greens
_vg_theme = Theme({
    # Core brand colors
    "vg-border": "#FFC300",          # sunflower yellow
    "vg-title": "bold #FFC300",
    "vg-panel": "#F5F1E6",          # warm white text
    # Text accents
    "vg-primary": "#274690",        # ultramarine blue (for primary info)
    "vg-info": "#90CAF9",           # sky blue
    "vg-accent": "#FFC300",         # sunflower accent
    "vg-muted": "#6C757D",          # neutral gray
    # Status
    "vg-ok": "bold #4C9A2A",       # sap green
    "vg-warn": "bold #CC7722",     # ochre
    "vg-error": "bold #D1495B",    # warm red
})

console = Console(theme=_vg_theme)

class StalkerBuildConfigError(Exception):
    pass

class StalkerBuildConfig:
    SIMD_OPTIONS = {"avx2", "avx512", "esp-dsp", "auto", "none"}
    BUILD_TYPES = {"Release", "Debug", "RelWithDebInfo", "MinSizeRel", "AllGasNoBrakes"}

    # Flat, ordered list for summary
    SUMMARY_FIELDS = [
        ("build_directory", "General build directory"),
        ("build_target", "Build target"),
        ("build_target_reason", "Target selection"),
        ("build_type", "Build type"),
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
        ("allGasNoBrakes", "ALL GAS NO BRAKES"),
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
        self.build_type = self._parseConfigArgument("build_type", "Debug", str)
        if self.build_type not in self.BUILD_TYPES:
            raise StalkerBuildConfigError(f"Invalid build_type '{self.build_type}'. Supported: {self.BUILD_TYPES}")

        self.allGasNoBrakes = (self.build_type == "AllGasNoBrakes")
        self._cmake_build_type = "Release" if self.allGasNoBrakes else self.build_type

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
        self.unroll_factor = int(self.raw.get("unroll_factor", 4))
        if self.unroll_factor <= 0 or (self.unroll_factor & (self.unroll_factor - 1)) != 0:
            raise StalkerBuildConfigError(f"Error: unroll_factor must be a positive power of 2 (got {self.unroll_factor})")

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
        # Project toggles
        self._cmake_args.append(f"-DSTALKER_BUILD_TESTS={'ON' if self.build_tests else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_BUILD_BENCHMARKS={'ON' if self.build_benchmarks else 'OFF'}")
        # SIMD
        if self.simd_enable:
            if self.simd_default_instructions == "none":
                self._cmake_args.append("-DSTALKER_SIMD_ENABLE=OFF")
            else:
                self._cmake_args.append("-DSTALKER_SIMD_ENABLE=ON")
            self._cmake_args.append(f"-DSTALKER_SIMD_INSTRUCTION_SET={self.simd_default_instructions}")
        else:
            self._cmake_args.append("-DSTALKER_SIMD_ENABLE=OFF")
        # Alignment
        self._cmake_args.append(f"-DSTALKER_ALIGNMENT={self.alignment}")
        # Loop unrolling
        self._cmake_args.append(f"-DSTALKER_UNROLL_FACTOR={self.unroll_factor}")
        # Threading
        self._cmake_args.append(f"-DSTALKER_THREADING_ENABLE={'ON' if self.threading_enable else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_MAX_THREADS_ENABLE={'ON' if self.threading_max_threads_enable else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_NUM_THREADS={self.threading_num_threads if self.threading_num_threads else 0}")
        self._cmake_args.append(f"-DSTALKER_THREADING_STD_ENABLE={'ON' if self.threading_std_enable else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_POSIX_ENABLE={'ON' if self.threading_posix_enable else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_POSIX_SMT_ENABLE={'ON' if self.enableHyperthreading else 'OFF'}")
        # Perf
        if self.allGasNoBrakes:
            self._cmake_args.append("-DSTALKER_ALL_GAS_NO_BRAKES=ON")
        # PAPI support
        papi_dir = self.raw.get("papiDir", None)
        if papi_dir:
            self._cmake_args.append(f"-DCMAKE_PREFIX_PATH={papi_dir}")

    def as_cmake_args(self):
        return list(self._cmake_args)

    def get_flat_summary(self):
        rows = []
        for field, desc in self.SUMMARY_FIELDS:
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
            "--config", self.cfg.build_type,
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
        "  • [vg-info]build_type[/vg-info]                One of: Release, Debug, RelWithDebInfo, MinSizeRel, AllGasNoBrakes.\n"
        "  • [vg-info]build_jobs[/vg-info]                Parallel build jobs (integer).\n"
        "  • [vg-info]build_tests[/vg-info]               true/false → -DSTALKER_BUILD_TESTS.\n"
        "  • [vg-info]build_benchmarks[/vg-info]          true/false → -DSTALKER_BUILD_BENCHMARKS.\n"
        "  • [vg-info]alignment[/vg-info]                 Positive power-of-two (e.g., 64).\n"
        "  • [vg-info]simd_enable[/vg-info]               true/false → enable SIMD at configure time.\n"
        "  • [vg-info]simd_default_instructions[/vg-info] avx2 | avx512 | esp-dsp | auto | none.\n"
        "  • [vg-info]unroll_enable[/vg-info]             true/false → metaprogram unrolling.\n"
        "  • [vg-info]unroll_factor[/vg-info]             Positive power-of-two (ignored if unroll disabled).\n"
        "  • [vg-info]threading_enable[/vg-info]          true/false.\n"
        "  • [vg-info]threading_num_threads[/vg-info]     Thread count (0 = auto).\n"
        "  • [vg-info]threading_std_enable[/vg-info]      true/false → std::thread backend.\n"
        "  • [vg-info]threading_posix_enable[/vg-info]    true/false → pthread backend.\n"
        "  • [vg-info]enableHyperthreading[/vg-info]      true/false (only with pthread backend).\n"
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
    args = parser.parse_args()

    if args.step == "help":
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
