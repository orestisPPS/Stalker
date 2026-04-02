"""Loader for C++ benchmark JSON logs."""
import os
import json
import glob
from dataclasses import dataclass, field
from typing import Dict, List, Optional

import numpy as np


@dataclass
class BenchmarkSeries:
    name: str
    values: List[float]
    compare_over: List[str] = field(default_factory=list)

    @property
    def mean(self) -> float:
        return float(np.mean(self.values)) if self.values else 0.0

    @property
    def std(self) -> float:
        return float(np.std(self.values)) if self.values else 0.0

    @property
    def median(self) -> float:
        return float(np.median(self.values)) if self.values else 0.0


@dataclass
class BenchmarkLog:
    filepath: str
    parameters: Dict[str, str] = field(default_factory=dict)
    measurement_sets: Dict[str, Dict[str, BenchmarkSeries]] = field(default_factory=dict)

    @property
    def filename(self) -> str:
        return os.path.basename(self.filepath)

    @property
    def size(self) -> Optional[float]:
        s = self.parameters.get("Size", "")
        try:
            return float(s)
        except ValueError:
            return None


class BenchmarkLoader:
    def __init__(self, logs_dir: str):
        self.logs_dir = logs_dir

    def list_logs(self) -> List[str]:
        return sorted(glob.glob(os.path.join(self.logs_dir, "*.json")))

    def load(self, filepath: str) -> Optional[BenchmarkLog]:
        try:
            with open(filepath, "r") as f:
                data = json.load(f)
        except Exception:
            return None

        params = data.get("_parameters", {})
        msets_raw = data.get("_measurementSets", {})
        msets = {}
        for unit, series_dict in msets_raw.items():
            if not isinstance(series_dict, dict):
                continue
            msets[unit] = {}
            for name, sdata in series_dict.items():
                if not isinstance(sdata, dict):
                    continue
                msets[unit][name] = BenchmarkSeries(
                    name=name,
                    values=sdata.get("values", []),
                    compare_over=sdata.get("compareOver", []),
                )
        return BenchmarkLog(filepath=filepath, parameters=params, measurement_sets=msets)

    def load_all(self) -> List[BenchmarkLog]:
        logs = []
        for fp in self.list_logs():
            log = self.load(fp)
            if log:
                logs.append(log)
        return logs

    def group_by_operation(self) -> Dict[str, List[BenchmarkLog]]:
        """Group logs by operation prefix (e.g. 'copy', 'add')."""
        groups: Dict[str, List[BenchmarkLog]] = {}
        for log in self.load_all():
            op = os.path.basename(log.filepath).split("_")[0]
            groups.setdefault(op, []).append(log)
        for v in groups.values():
            v.sort(key=lambda l: l.size or 0)
        return groups
