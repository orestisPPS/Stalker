"""Configuration models for the Stalker dashboard."""
from dataclasses import dataclass, field
from typing import Optional
from enum import Enum


class SIMDType(str, Enum):
    AVX2 = "AVX2"
    AVX512 = "AVX512"


class StorePolicy(str, Enum):
    Cached = "Cached"
    Streamed = "Streamed"


class AlignmentPolicy(str, Enum):
    Auto = "Auto"
    Aligned = "Aligned"
    Unaligned = "Unaligned"


class PrefetchHint(str, Enum):
    NONE = "None"
    T0 = "T0"


class ILPPolicy(str, Enum):
    Interleaved = "Interleaved"
    Grouped = "Grouped"


@dataclass
class SIMDConfig:
    simd_type: SIMDType = SIMDType.AVX2
    store: StorePolicy = StorePolicy.Cached
    alignment: AlignmentPolicy = AlignmentPolicy.Auto
    prefetch: PrefetchHint = PrefetchHint.NONE
    ilp: ILPPolicy = ILPPolicy.Interleaved

    @property
    def label(self) -> str:
        return f"{self.simd_type.value} | {self.store.value} | {self.alignment.value} | {self.prefetch.value} | {self.ilp.value}"

    def matches(self, config_name: str) -> bool:
        return self.label.lower() == config_name.strip().lower()


@dataclass
class TestParameters:
    size: int = 1_000_000
    dtypes: list = field(default_factory=lambda: ["float32", "float64", "int32", "uint32", "int16"])
    boundaries: list = field(default_factory=lambda: ["Aligned (64B)", "Unaligned"])
    suites: list = field(default_factory=lambda: ["memory", "math"])


@dataclass
class BuildConfig:
    project_root: str = ""
    build_dir: str = "build"
    venv_python: str = ""
    build_profile: str = "release"
    build_tests: bool = True
    build_benchmarks: bool = True
    build_examples: bool = True
    simd_enable: bool = True
    simd_instructions: str = "avx2"
    threading_enable: bool = True
    jobs: int = 16
