"""Result models for test and benchmark data."""
from dataclasses import dataclass, field
from typing import Optional
import time


@dataclass
class TestResult:
    op: str
    status: str  # PASS / FAIL / INTERCEPTED
    time_us: float
    py_time_us: float = 0.0
    err: float = 0.0
    msg: str = ""

    @property
    def passed(self) -> bool:
        return self.status == "PASS"

    @property
    def speedup(self) -> Optional[float]:
        if self.py_time_us > 0 and self.time_us > 0:
            return self.py_time_us / self.time_us
        return None


@dataclass
class SuiteResults:
    suite_name: str
    config_name: str
    dtype: str
    boundary: str
    results: list = field(default_factory=list)  # list[TestResult]
    timestamp: float = field(default_factory=time.time)

    @property
    def all_passed(self) -> bool:
        return all(r.passed for r in self.results)

    @property
    def all_intercepted(self) -> bool:
        return all(r.status == "INTERCEPTED" for r in self.results)

    @property
    def fail_count(self) -> int:
        return sum(1 for r in self.results if r.status == "FAIL")


@dataclass
class SessionResults:
    """Container for an entire test run."""
    suites: list = field(default_factory=list)  # list[SuiteResults]
    started_at: float = field(default_factory=time.time)
    finished_at: float = 0.0
    build_log: str = ""

    @property
    def total_fails(self) -> int:
        return sum(s.fail_count for s in self.suites)

    @property
    def total_pass(self) -> int:
        return sum(1 for s in self.suites if s.all_passed)

    @property
    def total_intercepted(self) -> int:
        return sum(1 for s in self.suites if s.all_intercepted)

    def filter(self, config_name: str = "", dtype: str = "", suite_name: str = "") -> list:
        out = self.suites
        if config_name:
            out = [s for s in out if config_name.lower() in s.config_name.lower()]
        if dtype:
            out = [s for s in out if s.dtype == dtype]
        if suite_name:
            out = [s for s in out if suite_name.lower() in s.suite_name.lower()]
        return out
