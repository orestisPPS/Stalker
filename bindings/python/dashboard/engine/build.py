"""Build engine — runs C++ build and Python binding install from the dashboard."""
import subprocess
import os
import json
import threading
from dataclasses import dataclass
from typing import Callable, Optional


@dataclass
class BuildResult:
    success: bool
    log: str
    return_code: int


class BuildEngine:
    def __init__(self, project_root: str):
        self.root = project_root
        self.build_json = os.path.join(project_root, "stalker-build.json")
        self.venv_python = os.path.join(project_root, "stalker-venv", "bin", "python")

    # ── blocking run ─────────────────────────────────────────────────
    def _run(self, cmd: list, cwd: str = None, env: dict = None) -> BuildResult:
        merged_env = os.environ.copy()
        if env:
            merged_env.update(env)
        try:
            proc = subprocess.run(
                cmd, cwd=cwd or self.root, capture_output=True, text=True,
                timeout=600, env=merged_env,
            )
            log = proc.stdout + "\n" + proc.stderr
            return BuildResult(success=proc.returncode == 0, log=log, return_code=proc.returncode)
        except subprocess.TimeoutExpired:
            return BuildResult(success=False, log="Build timed out after 600s", return_code=-1)
        except Exception as e:
            return BuildResult(success=False, log=str(e), return_code=-1)

    # ── streaming run (line-by-line callback) ────────────────────────
    def _run_streaming(self, cmd: list, on_line: Callable[[str], None],
                       cwd: str = None, env: dict = None) -> BuildResult:
        merged_env = os.environ.copy()
        if env:
            merged_env.update(env)
        lines: list[str] = []
        try:
            proc = subprocess.Popen(
                cmd, cwd=cwd or self.root,
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                text=True, bufsize=1, env=merged_env,
            )

            def _read():
                for raw in iter(proc.stdout.readline, ""):
                    lines.append(raw)
                    on_line(raw)
                proc.stdout.close()

            reader = threading.Thread(target=_read, daemon=True)
            reader.start()
            proc.wait(timeout=600)
            reader.join(timeout=5)
            log = "".join(lines)
            return BuildResult(success=proc.returncode == 0, log=log, return_code=proc.returncode)
        except subprocess.TimeoutExpired:
            proc.kill()
            return BuildResult(success=False, log="".join(lines) + "\nBuild timed out after 600s", return_code=-1)
        except Exception as e:
            return BuildResult(success=False, log=str(e), return_code=-1)

    # ── config I/O ───────────────────────────────────────────────────
    def read_build_config(self) -> dict:
        try:
            with open(self.build_json, "r") as f:
                return json.load(f)
        except Exception:
            return {}

    def write_build_config(self, cfg: dict):
        with open(self.build_json, "w") as f:
            json.dump(cfg, f, indent=2)

    # ── C++ commands ─────────────────────────────────────────────────
    def cpp_configure(self, on_line: Optional[Callable] = None) -> BuildResult:
        cmd = [self.venv_python, "Tools/build.py", "configure"]
        return self._run_streaming(cmd, on_line) if on_line else self._run(cmd)

    def cpp_build(self, on_line: Optional[Callable] = None) -> BuildResult:
        cmd = [self.venv_python, "Tools/build.py", "build"]
        return self._run_streaming(cmd, on_line) if on_line else self._run(cmd)

    def cpp_clean_build(self, on_line: Optional[Callable] = None) -> BuildResult:
        cmd = [self.venv_python, "Tools/build.py", "clean-build"]
        return self._run_streaming(cmd, on_line) if on_line else self._run(cmd)

    def cpp_clean(self, on_line: Optional[Callable] = None) -> BuildResult:
        cmd = [self.venv_python, "Tools/build.py", "clean"]
        return self._run_streaming(cmd, on_line) if on_line else self._run(cmd)

    # ── Python commands ──────────────────────────────────────────────
    def pip_install_bindings(self, on_line: Optional[Callable] = None) -> BuildResult:
        cmd = [self.venv_python, "-m", "pip", "install", "-e", "."]
        return self._run_streaming(cmd, on_line) if on_line else self._run(cmd)

    def pip_clean_install_bindings(self, on_line: Optional[Callable] = None) -> BuildResult:
        """Uninstall then reinstall in one streaming session."""
        lines: list[str] = []
        def _collect(line):
            lines.append(line)
            if on_line:
                on_line(line)

        r1 = self._run_streaming(
            [self.venv_python, "-m", "pip", "uninstall", "-y", "stalker"], _collect)
        if on_line:
            on_line("--- reinstalling ---\n")
        lines.append("--- reinstalling ---\n")
        r2 = self._run_streaming(
            [self.venv_python, "-m", "pip", "install", "-e", "."], _collect)
        log = "".join(lines)
        return BuildResult(success=r2.success, log=log, return_code=r2.return_code)

    def pip_uninstall(self, on_line: Optional[Callable] = None) -> BuildResult:
        cmd = [self.venv_python, "-m", "pip", "uninstall", "-y", "stalker"]
        return self._run_streaming(cmd, on_line) if on_line else self._run(cmd)
