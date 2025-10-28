import argparse
import json
import os
import shutil
import subprocess
import sys
from rich.console import Console
from rich.panel import Panel
from rich.table import Table

console = Console()

class StalkerBuildConfigError(Exception):
    pass

class StalkerBuildConfig:
    SIMD_OPTIONS = {"avx2", "avx512", "esp-dsp", "auto", "none"}
    BUILD_TYPES = {"Release", "Debug", "RelWithDebInfo", "MinSizeRel", "AllGasNoBrakes"}

    # Flat, ordered list for summary
    SUMMARY_FIELDS = [
        ("buildDir", "General build directory"),
        ("target", "Build target"),
        ("config", "CMake config"),
        ("buildType", "Build type"),
        ("jobs", "Build jobs"),
        ("enableSimd", "Enable SIMD"),
        ("simdInstructionSet", "SIMD instruction set"),
        ("alignment", "Memory alignment"),
        ("enableLoopUnroll", "Enable loop unrolling"),
        ("unrollFactor", "Loop unroll factor"),
        ("enableThreading", "Enable threading"),
        ("enableMaxThreads", "Enable max threads"),
        ("numThreads", "Number of threads"),
        ("enableThreadSTD", "Enable std::thread"),
        ("enableThreadPOSIX", "Enable POSIX threads"),
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
        self.buildDir = self._parseConfigArgument("buildDir", "build", str)
        if not self.buildDir or not isinstance(self.buildDir, str):
            raise StalkerBuildConfigError("Invalid buildDir: must be a non-empty string.")
        self.buildDir = os.path.abspath(os.path.normpath(self.buildDir))
        if os.path.isfile(self.buildDir):
            raise StalkerBuildConfigError(f"buildDir '{self.buildDir}' is a file, must be a directory.")
        # Optional: enforce that buildDir is not within the source dir if you want

        # Build jobs
        self.jobs = int(self.raw.get("jobs", os.cpu_count() or 4))
        if self.jobs <= 0 or self.jobs > 128:
            raise StalkerBuildConfigError(f"Invalid jobs count: {self.jobs}")

        # Build target and config
        self.target = self.raw.get("target", "TougeRacing")
        self.buildType = self._parseConfigArgument("buildType", "Debug", str)
        if self.buildType not in self.BUILD_TYPES:
            raise StalkerBuildConfigError(f"Invalid buildType '{self.buildType}'. Supported: {self.BUILD_TYPES}")
        self.config = self.raw.get("config", self.buildType)

        self.allGasNoBrakes = (self.buildType == "AllGasNoBrakes")
        self._cmake_build_type = "Release" if self.allGasNoBrakes else self.buildType

    def _parse_simd(self):
        self.enableSimd = self._parseConfigArgument("enableSimd", False, bool)
        self.simdInstructionSet = self._parseConfigArgument("simdInstructionSet", "auto", str).lower()
        if self.simdInstructionSet not in self.SIMD_OPTIONS:
            raise StalkerBuildConfigError(f"Invalid simdInstructionSet '{self.simdInstructionSet}'. Valid: {', '.join(self.SIMD_OPTIONS)}")

    def _parse_alignment(self):
        # Only allow positive, power-of-2 values. If -1 (not set), leave unset.
        self.alignment = int(self.raw.get("alignment", -1))
        if self.alignment != -1:
            if self.alignment <= 0 or (self.alignment & (self.alignment - 1)) != 0:
                raise StalkerBuildConfigError(f"Error: alignment must be a positive power of 2 (got {self.alignment})")

    def _parse_unroll(self):
        self.enableLoopUnroll = bool(self.raw.get("enableLoopUnroll", False))
        self.unrollFactor = int(self.raw.get("unrollFactor", 4))
        if self.enableLoopUnroll:
            if self.unrollFactor <= 0 or (self.unrollFactor & (self.unrollFactor - 1)) != 0:
                raise StalkerBuildConfigError(f"Error: unrollFactor must be a positive power of 2 (got {self.unrollFactor})")
        else:
            self.unrollFactor = 1  # always enforce 1 if not unrolling

    def _parse_threading(self):
        self.enableThreading = bool(self.raw.get("enableThreading", False))
        self.enableMaxThreads = bool(self.raw.get("enableMaxThreads", False))
        self.numThreads = int(self.raw.get("numThreads", 0))


        self.enableThreadSTD = bool(self.raw.get("enableThreadSTD", False))
        self.enableThreadPOSIX = bool(self.raw.get("enableThreadPOSIX", False))
        self.enableHyperthreading = bool(self.raw.get("enableHyperthreading", False))


        # Safety checks for threading options
        if self.enableThreadSTD and self.enableThreadPOSIX:
            raise StalkerBuildConfigError("Cannot enable both std::thread and POSIX threads at the same time.")
        if self.enableHyperthreading and self.enableThreadSTD:
            self.enableHyperthreading = False  # silently ignore
        if self.enableHyperthreading and not self.enableThreadPOSIX:
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
        # SIMD
        if self.enableSimd:
            if self.simdInstructionSet == "none":
                self._cmake_args.append("-DSTALKER_SIMD_ENABLE=OFF")
            else:
                self._cmake_args.append("-DSTALKER_SIMD_ENABLE=ON")
            self._cmake_args.append(f"-DSTALKER_SIMD_INSTRUCTION_SET={self.simdInstructionSet}")
        else:
            self._cmake_args.append("-DSTALKER_SIMD_ENABLE=OFF")
        # Alignment
        self._cmake_args.append(f"-DSTALKER_ALIGNMENT={self.alignment}")
        # Loop unrolling
        self._cmake_args.append(f"-DSTALKER_UNROLL_ENABLE={'ON' if self.enableLoopUnroll else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_UNROLL_FACTOR={self.unrollFactor}")
        # Threading
        self._cmake_args.append(f"-DSTALKER_THREADING_ENABLE={'ON' if self.enableThreading else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_ENABLE_MAX_THREADS={'ON' if self.enableMaxThreads else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_NUM_THREADS={self.numThreads if self.numThreads else 0}")
        self._cmake_args.append(f"-DSTALKER_THREADING_ENABLE_STDTHREAD={'ON' if self.enableThreadSTD else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_ENABLE_PTHREAD={'ON' if self.enableThreadPOSIX else 'OFF'}")
        self._cmake_args.append(f"-DSTALKER_THREADING_ENABLE_SMT={'ON' if self.enableHyperthreading else 'OFF'}")
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
        console.rule("[bold red]Clean Build Directory")
        if os.path.exists(self.cfg.buildDir):
            console.print(f"Removing build directory: [yellow]{self.cfg.buildDir}[/yellow]")
            shutil.rmtree(self.cfg.buildDir)
            console.print("[green]Build directory removed.[/green]")
        else:
            console.print("[yellow]Build directory does not exist, nothing to clean.[/yellow]")

    def run_setup(self):
        cmake_cmd = [
            "cmake", "-S", ".", "-B", self.cfg.buildDir
        ] + self.cfg.as_cmake_args()
        console.rule("[bold blue]CMake Configure (Setup)")
        console.print(Panel(" ".join(cmake_cmd), title="CMake Command", style="cyan"))
        try:
            res = subprocess.run(cmake_cmd, check=True)
        except subprocess.CalledProcessError as e:
            console.print(f"[bold red]CMake configure failed with exit code {e.returncode}[/bold red]")
            raise
        console.print("[bold green]CMake configure complete.[/bold green]")

    def run_build(self):
        build_cmd = [
            "cmake",
            "--build", self.cfg.buildDir,
            "--target", self.cfg.target,
            "--config", self.cfg.config,
            "-j", str(self.cfg.jobs)
        ]
        console.rule("[bold blue]CMake Build")
        console.print(Panel(" ".join(build_cmd), title="Build Command", style="cyan"))
        try:
            res = subprocess.run(build_cmd, check=True)
        except subprocess.CalledProcessError as e:
            console.print(f"[bold red]CMake build failed with exit code {e.returncode}[/bold red]")
            raise
        console.print("[bold green]Build complete.[/bold green]")

    def print_summary(self):
        table = Table(title="Stalker Build Configuration", show_lines=True)
        table.add_column("Setting", justify="right", style="bold green")
        table.add_column("Value", style="cyan")
        for k, v in self.cfg.get_flat_summary():
            table.add_row(k, v)
        console.print(Panel(table, title="Build Summary", style="bold blue"))

def print_help():
    console.print(Panel(
        "[bold yellow]Stalker Build System Help[/bold yellow]\n\n"
        "Commands (choose one):\n"
        "  [green]build[/green]         Just build, assuming prior setup (cmake --build)\n"
        "  [green]clean[/green]         Remove the build directory only\n"
        "  [green]clean-build[/green]   Clean build dir, then full setup+build (fresh build)\n"
        "  [green]full[/green]          Setup and build (without deleting build dir)\n"
        "  [green]help[/green]          Print this help message\n",
        title="Stalker Builder Help",
        style="bold blue"
    ))

def main():
    parser = argparse.ArgumentParser(
        description="Stalker Build Script (with rich output)",
        add_help=False
    )
    parser.add_argument(
        "step",
        choices=["build", "clean", "clean-build", "full", "help"],
        nargs="?",
        default="full",
        help="Which step to run: build, clean, clean-build, full, help (default: full)"
    )
    parser.add_argument(
        "--verbose", "-v", action="store_true",
        help="Enable verbose output for CMake and build"
    )
    args = parser.parse_args()

    if args.step == "help":
        print_help()
        sys.exit(0)

    try:
        config = StalkerBuildConfig()
        config.verbose = args.verbose  # inject
    except StalkerBuildConfigError as e:
        console.print(Panel(str(e), title="Configuration Error", style="bold red"))
        sys.exit(2)

    builder = StalkerBuilder(config)
    builder.print_summary()

    try:
        if args.step == "clean":
            builder.clean_build_dir()
        elif args.step == "build":
            builder.run_build()
        elif args.step == "clean-build":
            builder.clean_build_dir()
            builder.run_setup()
            builder.run_build()
        elif args.step == "full":
            builder.run_setup()
            builder.run_build()
    except Exception as e:
        console.print(Panel(f"Build step failed: {e}", title="Build Error", style="bold red"))
        sys.exit(3)

if __name__ == "__main__":
    main()
