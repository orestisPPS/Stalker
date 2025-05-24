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

class StalkerBuildConfig:
    SIMD_OPTIONS = {"avx2", "avx512", "neon", "esp-dsp", "none"}
    BUILD_TYPES = {"Release", "Debug", "RelWithDebInfo", "MinSizeRel", "AllGasNoBrakes"}

    def __init__(self, json_path="stalker-build.json"):
        self.raw = self._load_json(json_path)
        self._parse_config()

    def _load_json(self, path):
        try:
            with open(path, "r") as f:
                return json.load(f)
        except Exception as e:
            console.print(f"[bold red]Error reading {path}:[/bold red] {e}")
            sys.exit(1)

    def _parse_config(self):
        self.buildDir = self.raw.get("buildDir", "build")
        self.enableSimd = bool(self.raw.get("enableSimd", False))
        self.simdInstructionSet = self.raw.get("simdInstructionSet", "avx2").lower()
        self.buildType = self.raw.get("buildType", "Debug")
        self.jobs = int(self.raw.get("jobs", os.cpu_count() or 4))
        self.target = self.raw.get("target", "all")
        self.config = self.raw.get("config", self.buildType)
        self.unrollFactor = int(self.raw.get("unrollFactor", 4))
        self.enableOpenMP = bool(self.raw.get("enableOpenMP", False))
        self.enableInternalThreading = bool(self.raw.get("enableInternalThreading", False))

        # This is the only spot you have to handle AllGasNoBrakes
        self.allGasNoBrakes = self.buildType == "AllGasNoBrakes"
        self._cmake_build_type = "Release" if self.allGasNoBrakes else self.buildType

        if self.simdInstructionSet not in self.SIMD_OPTIONS:
            console.print(
                f"[bold red]Invalid simdInstructionSet '{self.simdInstructionSet}'. Supported:[/bold red] {', '.join(self.SIMD_OPTIONS)}"
            )
            sys.exit(1)
        if self.buildType not in self.BUILD_TYPES:
            console.print(
                f"[bold red]Invalid buildType '{self.buildType}'. Supported:[/bold red] {', '.join(self.BUILD_TYPES)}"
            )
            sys.exit(1)

    def as_cmake_args(self):
        args = [f"-DCMAKE_BUILD_TYPE={self._cmake_build_type}"]
        args.append(f"-DSTALKER_UNROLL_FACTOR={self.unrollFactor}")
        if self.enableSimd:
            args.append("-DSTALKER_ENABLE_SIMD=ON")
            args.append(f"-DSTALKER_SIMD_INSTRUCTION_SET={self.simdInstructionSet}")
        else:
            args.append("-DSTALKER_ENABLE_SIMD=OFF")
        if self.enableOpenMP:
            args.append("-DSTALKER_ENABLE_OPENMP=ON")
        else:
            args.append("-DSTALKER_ENABLE_OPENMP=OFF")
        if self.enableInternalThreading:
            args.append("-DSTALKER_ENABLE_INTERNAL_THREADING=ON")
        else:
            args.append("-DSTALKER_ENABLE_INTERNAL_THREADING=OFF")
        if self.allGasNoBrakes:
            args.append("-DSTALKER_ALL_GAS_NO_BRAKES=ON")
        return args

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
        res = subprocess.run(cmake_cmd)
        if res.returncode != 0:
            console.print("[bold red]CMake configure failed.[/bold red]")
            sys.exit(res.returncode)
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
        res = subprocess.run(build_cmd)
        if res.returncode != 0:
            console.print("[bold red]CMake build failed.[/bold red]")
            sys.exit(res.returncode)
        console.print("[bold green]Build complete.[/bold green]")

    def print_summary(self):
        table = Table(title="Build Configuration")
        for k in [
            "buildDir", "enableSimd", "simdInstructionSet", "buildType", "jobs",
            "target", "config", "unrollFactor", "enableOpenMP", "enableInternalThreading"
        ]:
            table.add_row(k, str(getattr(self.cfg, k)))
        console.print(table)

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
    args = parser.parse_args()

    if args.step == "help":
        print_help()
        sys.exit(0)

    config = StalkerBuildConfig()
    builder = StalkerBuilder(config)
    builder.print_summary()

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

if __name__ == "__main__":
    main()
