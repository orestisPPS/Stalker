#!/usr/bin/env python3
"""
Professional HPC Benchmark Plotter (Multi-file)
- OOP, CamelCase naming
- Accepts a directory. Parses all *.json. Errors if none.
- For each run:file -> plots per metric + stopwatches with median bars and std error bars
- Parameters printed on each figure (from that run)
- Saves per-run PNGs under OUT_DIR/<RunLabel> and an aggregated CSV with Run column
- Also saves optional combined comparison plots per metric across runs

Usage:
  python benchmark_plotter_multi.py --input_dir /path/to/jsons --out plots --dpi 140
"""

import os
import math
import json
import argparse
from dataclasses import dataclass, field
from typing import Dict, List, Any, Tuple

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker
try:
    from rich.console import Console
    from rich.table import Table
    from rich.panel import Panel
    _RICH_AVAILABLE = True
    _console = Console()
except Exception:
    _RICH_AVAILABLE = False
    _console = None
from datetime import datetime


# ---------------------------- Data classes ----------------------------

@dataclass
class MetricStats:
    count: int
    median: float
    mean: float
    std: float
    min: float
    max: float


@dataclass
class MetricSetResult:
    metric_name: str
    method_stats: Dict[str, MetricStats] = field(default_factory=dict)


# ---------------------------- Single-run Plotter ----------------------------

class BenchmarkPlotter:
    def __init__(self, data: Dict[str, Any], out_dir: str, dpi: int = 140, run_label: str = "", err: str = "std", err_scale: float = 1.0, log_y: bool = False):
        self.data = data
        self.out_dir = out_dir
        self.dpi = dpi
        self.run_label = run_label
        self.err = err  # 'std' | 'stderr' | 'none'
        self.err_scale = err_scale
        self.log_y = log_y
        os.makedirs(self.out_dir, exist_ok=True)

        self.parameters = self._extractParameters()
        self.measurement_sets = self._extractMeasurementSets()
        self.stopwatches = self._extractStopwatches()
        # Summary dataframe placeholder (set after plotting)
        self.summary_df = None
        # compareOver map captured per metric/method from JSON
        self.compare_over = self._extractCompareOver()

    # ---------- Small helpers ----------
    @staticmethod
    def _is_truthy(raw_flag: Any) -> bool:
        try:
            if isinstance(raw_flag, str):
                return raw_flag.strip().lower() in ("true", "1", "yes", "y", "t")
            if isinstance(raw_flag, (int, float)):
                return float(raw_flag) != 0.0
            if isinstance(raw_flag, bool):
                return raw_flag
            return bool(raw_flag)
        except Exception:
            return True

    # ---------- Extraction ----------
    def _extractParameters(self) -> Dict[str, str]:
        return self.data.get("_parameters", {}) or {}

    def _extractMeasurementSets(self) -> Dict[str, Dict[str, List[float]]]:
        sets = self.data.get("_measurementSets", {}) or {}
        legacy_plots = self.data.get("_measurementSetsPlot", {}) or {}
        clean_sets: Dict[str, Dict[str, List[float]]] = {}
        for metric, methods in sets.items():
            # Optional set-level plot flag inside the set object (keys: _plot or plot)
            if isinstance(methods, dict):
                set_plot_flag = True
                raw_set_flag = methods.get("_plot", methods.get("plot", None))
                # Only treat as a set flag if it's a scalar (not dict/list)
                if raw_set_flag is not None and not isinstance(raw_set_flag, (dict, list)):
                    set_plot_flag = self._is_truthy(raw_set_flag)
                if not set_plot_flag:
                    # Skip entire metric
                    continue
            # Supported schemas:
            # 1) New nested: { method: { "values": [...], "plot": bool } }
            # 2) Flat dict: { method: [values...] }
            # 3) Legacy list: [values...]  -> wrap as {"values": [...]}
            if isinstance(methods, dict):
                out: Dict[str, List[float]] = {}
                for m, v in methods.items():
                    # Skip set-level plot keys
                    if m in ("_plot", "plot") and not isinstance(v, list):
                        continue
                    # Case 1: nested object
                    if isinstance(v, dict) and "values" in v:
                        raw_flag = v.get("plot", True)
                        plot_flag = self._is_truthy(raw_flag)
                        if not plot_flag:
                            continue  # honor plot=false by skipping
                        vals = v.get("values", [])
                        out[m] = [float(x) for x in vals]
                    # Case 2: plain list
                    elif isinstance(v, list):
                        # If legacy top-level plot flags exist, honor them
                        metric_flags = legacy_plots.get(metric, {}) if isinstance(legacy_plots, dict) else {}
                        if isinstance(metric_flags, dict):
                            raw_flag = metric_flags.get(m, True)
                            flag = self._is_truthy(raw_flag)
                            if not flag:
                                continue
                        out[m] = [float(x) for x in v]
                    else:
                        # Unsupported entry; skip
                        continue
                if out:
                    clean_sets[metric] = out
            elif isinstance(methods, list):
                # Legacy schema: set -> [values...]; wrap as a single method "values"
                clean_sets[metric] = {"values": [float(x) for x in methods]}
            else:
                # Unsupported type; skip gracefully
                continue
        return clean_sets

    def _extractStopwatches(self) -> Dict[str, List[float]]:
        sw = self.data.get("_stopwatches", {}) or {}
        if not isinstance(sw, dict):
            return {}
        # Optional set-level plot flag inside stopwatches
        raw_flag = sw.get("_plot", sw.get("plot", None))
        if raw_flag is not None:
            # Only honor scalar flags
            if not isinstance(raw_flag, (dict, list)) and not self._is_truthy(raw_flag):
                return {}
        out: Dict[str, List[float]] = {}
        # Capture compareOver for stopwatches too
        cmp_sw: Dict[str, List[str]] = {}
        for k, v in sw.items():
            if k in ("_plot", "plot"):
                continue
            if isinstance(v, list):
                # Legacy flat list
                out[k] = [float(x) for x in v]
            elif isinstance(v, dict):
                # New schema: { values: [...], compareOver: [...] }
                vals = v.get("values", [])
                mflag = v.get("plot", True)
                if self._is_truthy(mflag):
                    out[k] = [float(x) for x in vals]
                try:
                    lst = v.get("compareOver", [])
                    if isinstance(lst, list):
                        cmp_sw[k] = [str(x) for x in lst if isinstance(x, (str, int, float))]
                except Exception:
                    pass
        # Store compareOver under the Stopwatches metric name, merging later in plotDirectory
        if cmp_sw:
            self._cmp_sw_extracted = cmp_sw
        else:
            self._cmp_sw_extracted = {}
        return out

    def _extractCompareOver(self) -> Dict[str, Dict[str, List[str]]]:
        sets = self.data.get("_measurementSets", {}) or {}
        cmp: Dict[str, Dict[str, List[str]]] = {}
        for metric, methods in sets.items():
            if not isinstance(methods, dict):
                continue
            inner: Dict[str, List[str]] = {}
            for m, v in methods.items():
                if m in ("_plot", "plot"):
                    continue
                if isinstance(v, dict) and "compareOver" in v:
                    try:
                        lst = v.get("compareOver", [])
                        if isinstance(lst, list):
                            inner[m] = [str(x) for x in lst if isinstance(x, (str, int, float))]
                    except Exception:
                        pass
            if inner:
                cmp[metric] = inner
        return cmp

    # ---------- Stats ----------
    @staticmethod
    def _computeStats(values: List[float]) -> MetricStats:
        arr = np.asarray(values, dtype=float)
        n = int(arr.size)
        if n == 0:
            return MetricStats(0, float("nan"), float("nan"), float("nan"), float("nan"), float("nan"))
        median = float(np.median(arr))
        mean = float(np.mean(arr))
        std = float(np.std(arr, ddof=1)) if n > 1 else 0.0
        mn = float(np.min(arr))
        mx = float(np.max(arr))
        return MetricStats(n, median, mean, std, mn, mx)

    def _computeAllStats(self) -> Tuple[List[MetricSetResult], MetricSetResult]:
        results: List[MetricSetResult] = []
        for metric, methods in self.measurement_sets.items():
            msr = MetricSetResult(metric_name=metric)
            for method, values in methods.items():
                msr.method_stats[method] = self._computeStats(values)
            results.append(msr)
        sw_res = MetricSetResult(metric_name="Stopwatches")
        for method, values in self.stopwatches.items():
            sw_res.method_stats[method] = self._computeStats(values)
        return results, sw_res

    # ---------- Formatting ----------
    def _parametersText(self) -> str:
        if not self.parameters:
            return "No parameters provided"
        lines = [f"{k}: {self.parameters[k]}" for k in sorted(self.parameters.keys())]
        return "\n".join(lines)

    @staticmethod
    def _sanitizeFileName(name: str) -> str:
        return "".join(ch if ch.isalnum() or ch in (" ", "_", "-") else "_" for ch in name).replace(" ", "_")

    # ---------- Plotting ----------
    def _plotMetricSet(self, result: MetricSetResult) -> str:
        methods = list(result.method_stats.keys())
        medians = [result.method_stats[m].median for m in methods]
        # stds not needed directly here; error bars handled in bar constructor via yerr

        fig = plt.figure(figsize=(8, 4.5), dpi=self.dpi)
        ax = fig.add_subplot(111)

        # Determine error bars according to mode
        if self.err == "none":
            yerr = None
        elif self.err == "stderr":
            # standard error = std / sqrt(n)
            n_list = [max(1, int(result.method_stats[m].count)) for m in methods]
            yerr = [ (result.method_stats[m].std / max(1.0, math.sqrt(float(n)))) * self.err_scale for m, n in zip(methods, n_list) ]
        else:  # 'std'
            yerr = [ result.method_stats[m].std * self.err_scale for m in methods ]

        x = np.arange(len(methods))
        ax.bar(x, medians, yerr=yerr, capsize=4)
        ax.set_xticks(x, methods, rotation=20, ha="right")
        title = result.metric_name if not self.run_label else f"{result.metric_name} — {self.run_label}"
        ax.set_title(title)
        ax.set_ylabel("Median ± Std")
        ax.grid(True, axis="y", linestyle=":", linewidth=0.8)
        # Finer y-axis ticks and minor grid
        try:
            ax.yaxis.set_major_locator(mticker.MaxNLocator(nbins=10, min_n_ticks=6))
            if self.log_y:
                # For log scale, enable minor ticks automatically
                ax.minorticks_on()
            else:
                ax.yaxis.set_minor_locator(mticker.AutoMinorLocator(2))
            ax.grid(True, axis='y', which='minor', linestyle=':', linewidth=0.5, alpha=0.5)
        except Exception:
            pass
        if self.log_y:
            try:
                ax.set_yscale('log')
            except Exception:
                pass
        # Keep only built-in errorbar indicators (capsize on yerr). Custom overlays removed.

        # Parameters box
        ax.text(
            1.02, 0.5, self._parametersText(),
            transform=ax.transAxes,
            va="center", ha="left", fontsize=9,
            bbox=dict(boxstyle="round", facecolor="white", alpha=0.8, edgecolor="lightgray"),
        )

        plt.tight_layout()
        fname = f"{self._sanitizeFileName(result.metric_name)}.png"
        fpath = os.path.join(self.out_dir, fname)
        fig.savefig(fpath, bbox_inches="tight")
        plt.close(fig)
        return fpath

    def _plotStopwatches(self, sw_result: MetricSetResult) -> str:
        sw_result = MetricSetResult(metric_name="Stopwatches (timer ticks)", method_stats=sw_result.method_stats)
        return self._plotMetricSet(sw_result)

    # ---------- Public API ----------
    def plotAll(self) -> Dict[str, Any]:
        metric_results, sw_result = self._computeAllStats()

        # Summary table
        records = []
        for res in metric_results + ([sw_result] if sw_result.method_stats else []):
            for method, stats in res.method_stats.items():
                records.append({
                    "Run": self.run_label or "run",
                    "Metric": res.metric_name,
                    "Method": method,
                    "N": stats.count,
                    "Median": stats.median,
                    "Mean": stats.mean,
                    "Std": stats.std,
                    "Min": stats.min,
                    "Max": stats.max,
                })
        cols = ["Run", "Metric", "Method", "N", "Median", "Mean", "Std", "Min", "Max"]
        if len(records) == 0:
            # No metrics found for this JSON; create an empty DataFrame
            self.summary_df = pd.DataFrame(columns=cols)
            summary_csv = os.path.join(self.out_dir, "summary_stats.csv")
            # Write empty CSV with header for consistency
            self.summary_df.to_csv(summary_csv, index=False)
        else:
            self.summary_df = pd.DataFrame.from_records(records)
            self.summary_df = self.summary_df.set_index(["Run", "Metric", "Method"]).sort_index()
            summary_csv = os.path.join(self.out_dir, "summary_stats.csv")
            self.summary_df.to_csv(summary_csv)

        outputs = {"plots": [], "summary_csv": summary_csv}
        for res in metric_results:
            outputs["plots"].append(self._plotMetricSet(res))
        if sw_result.method_stats:
            outputs["plots"].append(self._plotStopwatches(sw_result))
        return outputs


# ---------------------------- Multi-run Orchestrator ----------------------------

class MultiBenchmarkPlotter:
    def __init__(self, input_dir: str, out_dir: str = "plots", dpi: int = 140, show: bool = False, err: str = "std", err_scale: float = 1.0, log_y: bool = False):
        self.input_dir = input_dir
        self.out_dir = out_dir
        self.dpi = dpi
        self.show = show
        self.err = err
        self.err_scale = err_scale
        self.log_y = log_y
        os.makedirs(self.out_dir, exist_ok=True)
        # Aggregated raw values across all files, honoring plot flags
        # Shape: { metric_name: { method_name: [values...] } }
        self._agg_values: Dict[str, Dict[str, List[float]]] = {}
        # Preserve first-seen order (from JSON) for metrics and methods
        self._metric_order: List[str] = []
        self._method_order: Dict[str, List[str]] = {}
        # Parameters captured from the first valid benchmark JSON
        self._first_run_parameters: Dict[str, str] = {}
        # compareOver associations aggregated (first-seen wins)
        self._compare_over = {}
        # Accumulate computed percent differences across metrics for CSV export
        self._diff_records = []  # type: List[Dict[str, Any]]
        # Parsed run metadata (operation, type, unroll, size)
        self._run_meta: Dict[str, Dict[str, Any]] = {}
        # Scaling plots collected paths
        self._scaling_plots: List[str] = []

    # ---------------------------- Run label parsing ----------------------------
    @staticmethod
    def _parse_run_label(run_label: str) -> Dict[str, Any]:
        """Parse labels in the general form operation_type_unroll_size.
        Robust to variants like ..._u<digits> and ..._s<digits>.
        Returns dict with keys: operation (str), type (str|None), unroll (int|None), size (int|None),
        dtype (str|None in {"d","f"}), sizeType (str|None in {"S","L","ND"}).
        """
        tokens = run_label.split("_") if run_label else []
        if not tokens:
            return {"operation": None, "type": None, "unroll": None, "size": None, "dtype": None, "sizeType": None}
        operation = tokens[0]
        type_tokens: List[str] = []
        unroll = None
        size = None
        dtype = None  # 'd' or 'f'
        size_type = None  # 'S' | 'L' | 'ND'
        prev_tok = None
        # Prefer tokens with explicit markers first
        for tok in tokens[1:]:
            low = tok.lower()
            if (low.startswith("u") and low[1:].isdigit()) or low.startswith("unroll"):
                # u<digits> or unroll<digits>
                digits = ''.join(ch for ch in low if ch.isdigit())
                if digits:
                    try:
                        unroll = int(digits)
                    except Exception:
                        unroll = None
                continue
            if low.startswith("s") and low[1:].isdigit():
                try:
                    size = int(low[1:])
                except Exception:
                    size = None
                continue
            # Size type explicitly as separate token: 'S' or 'L' (case-insensitive)
            if tok in ("S", "L", "s", "l"):
                size_type = tok.upper()
                continue
            # DType immediately after a 't' token: expect 'd' or 'f'
            if prev_tok and prev_tok.lower() == 't':
                if low in ("d", "double"):
                    dtype = 'd'
                    prev_tok = tok
                    continue
                if low in ("f", "float"):
                    dtype = 'f'
                    prev_tok = tok
                    continue
            # Otherwise consider part of type descriptor
            # Ignore bare numeric tokens (likely timestamps or counters)
            if not tok.isdigit():
                type_tokens.append(tok)
            prev_tok = tok
        type_str = "_".join(type_tokens) if type_tokens else None
        # Default sizeType to ND if not explicitly set
        if size_type is None:
            size_type = "ND"
        return {"operation": operation, "type": type_str, "unroll": unroll, "size": size, "dtype": dtype, "sizeType": size_type}

    @staticmethod
    def _findJsonFiles(directory: str) -> List[str]:
        return sorted(
            [os.path.join(directory, f) for f in os.listdir(directory)
             if f.lower().endswith(".json") and os.path.isfile(os.path.join(directory, f))]
        )

    @staticmethod
    def _loadJson(path: str) -> Dict[str, Any]:
        with open(path, "r", encoding="utf-8") as f:
            return json.load(f)

    @staticmethod
    def _labelFromFilename(path: str) -> str:
        base = os.path.basename(path)
        return os.path.splitext(base)[0]

    # Simplified: removed unused helpers _collectAllMetrics and _availableMethodsForMetric

    def _plotMedianByMethod(self, combined_df: pd.DataFrame, metric: str, out_dir: str, quiet: bool = False) -> str:
        # combined_df has per-run rows with columns: Run, Metric, Method, Median
        df_m = combined_df[combined_df["Metric"] == metric].copy()
        if df_m.empty:
            return ""

        # Determine methods order as they appear in JSON (first-seen across files)
        if metric in self._method_order:
            seen_set = set(df_m["Method"].unique().tolist())
            methods = [m for m in self._method_order[metric] if m in seen_set]
        else:
            # Fall back to first-occurrence order in the aggregated dataframe
            methods = list(dict.fromkeys(df_m["Method"].tolist()))
        # Median of per-run medians for each method
        med_by_method = df_m.groupby("Method")["Median"].median().reindex(methods)
        # Print medians table for this metric (suppressed when quiet=True)
        if not quiet:
            try:
                if _RICH_AVAILABLE:
                    tbl = Table(title=f"Medians — {metric}", show_lines=False, header_style="bold cyan")
                    tbl.add_column("#", justify="right", style="dim")
                    tbl.add_column("Method")
                    tbl.add_column("Median", justify="right")
                    for idx, m in enumerate(methods, start=1):
                        val = med_by_method.get(m)
                        txt = f"{val:.6g}" if (val is not None and np.isfinite(val)) else "NA"
                        tbl.add_row(str(idx), m, txt)
                    _console.print(tbl)
                else:
                    print(f"\nMedians — {metric}")
                    for idx, m in enumerate(methods, start=1):
                        val = med_by_method.get(m)
                        txt = f"{val:.6g}" if (val is not None and np.isfinite(val)) else "NA"
                        print(f"  {idx:>2}. {m}: {txt}")
            except Exception:
                pass
        # Determine label N: for a single run, show number of samples per method (from 'N');
        # for multiple runs, show runs and an approximate per-run N (median across methods/runs).
        n_runs = df_m["Run"].nunique()
        if n_runs == 1:
            try:
                n_samples = int(df_m["N"].max())
            except Exception:
                n_samples = int(df_m.shape[0])
            legend_label = f"Median (N={n_samples})"
        else:
            try:
                totals = df_m.groupby("Method")["N"].sum()
                if totals.nunique() == 1:
                    total_n = int(totals.iloc[0])
                    legend_label = f"Median (N={total_n})"
                else:
                    approx_total = int(totals.median())
                    legend_label = f"Median (N≈{approx_total})"
            except Exception:
                approx_total = int(df_m.shape[0] / max(1, len(methods)))
                legend_label = f"Median (N≈{approx_total})"

        # Create a 1x2 figure: main plot (bars) + comparison subplot (percent differences)
        fig, (ax, ax_cmp) = plt.subplots(1, 2, figsize=(13, 5), dpi=self.dpi, gridspec_kw={"width_ratios": [3, 1.6]})

        # Determine error bars according to mode for aggregated medians across runs
        # Prefer computing standard deviation directly from concatenated raw values across ALL files and iterations.
        if self.err == "none":
            yerr = None
        else:
            # Try to use aggregated raw values if available
            yerr = []
            total_counts: List[int] = []
            have_raw = metric in self._agg_values and isinstance(self._agg_values[metric], dict)
            if have_raw:
                for m in methods:
                    vals = self._agg_values[metric].get(m, [])
                    arr = np.asarray(vals, dtype=float)
                    n = int(arr.size)
                    total_counts.append(n)
                    if n <= 1:
                        yerr.append(0.0)
                    else:
                        std_all = float(np.std(arr, ddof=1))
                        if self.err == "stderr":
                            yerr.append((std_all / math.sqrt(float(n))) * self.err_scale)
                        else:
                            yerr.append(std_all * self.err_scale)
            else:
                # Fallback to pooled std computed from per-run stats (mathematically equivalent to std over all values)
                pooled_std_list = []
                for m in methods:
                    sub = df_m[df_m["Method"] == m]
                    n_i = sub["N"].astype(float).values
                    mu_i = sub["Mean"].astype(float).values
                    s_i = sub["Std"].astype(float).values
                    Ntot = int(np.sum(n_i)) if n_i.size else 0
                    total_counts.append(Ntot)
                    if Ntot <= 1:
                        pooled_std_list.append(0.0)
                        continue
                    mu = float(np.sum(n_i * mu_i) / np.sum(n_i))
                    ss_within = float(np.sum((n_i - 1.0) * (s_i ** 2)))
                    ss_between = float(np.sum(n_i * ((mu_i - mu) ** 2)))
                    var_pooled = (ss_within + ss_between) / (Ntot - 1.0)
                    pooled_std = math.sqrt(max(0.0, var_pooled))
                    pooled_std_list.append(pooled_std)
                pooled_std_arr = np.array(pooled_std_list, dtype=float)
                if self.err == "stderr":
                    denom = np.sqrt(np.maximum(1.0, np.array(total_counts, dtype=float)))
                    yerr = (pooled_std_arr / denom) * self.err_scale
                else:
                    yerr = pooled_std_arr * self.err_scale

        x = np.arange(len(methods))
        # Build legend: median center and error bar meaning
        full_label = f"{legend_label}"
        bars = ax.bar(x, med_by_method.values, yerr=yerr, capsize=3, label=full_label)

        ax.set_xticks(x, methods, rotation=20, ha="right")
        ax.set_title(f"{metric}")
        ax.set_ylabel(metric)
        ax.grid(True, axis="y", linestyle=":", linewidth=0.8)
        # Finer y-axis ticks and minor grid
        try:
            ax.yaxis.set_major_locator(mticker.MaxNLocator(nbins=10, min_n_ticks=6))
            if self.log_y:
                ax.minorticks_on()
            else:
                ax.yaxis.set_minor_locator(mticker.AutoMinorLocator(2))
            ax.grid(True, axis='y', which='minor', linestyle=':', linewidth=0.5, alpha=0.5)
        except Exception:
            pass
        if self.log_y:
            try:
                ax.set_yscale('log')
            except Exception:
                pass
        ax.legend(fontsize=9, loc="best")

        # Build a scientific comparison subplot to the right
        cmp_map = self._compare_over.get(metric, {})
        med_series = med_by_method

        # Only include methods that actually declare compareOver baselines
        comp_methods = [m for m in methods if (cmp_map.get(m, []) or [])]

        # Build ordered list of baseline columns, preserving first-seen order across methods
        baseline_cols: List[str] = []
        for m in comp_methods:
            for b in (cmp_map.get(m, []) or []):
                if b not in baseline_cols:
                    baseline_cols.append(b)

        # Helper to shorten method names (remove common prefix)
        def _short_method(m: str) -> str:
            s = m
            if s.lower().startswith("stalker "):
                s = s[len("stalker "):]
            return s

    # Build the table data
        rows: List[List[str]] = []

        def _compute_cell(m_val: float, b_val: float) -> str:
            """Single formula for all metrics: Δ% = 100 * (m - b) / b.
            If baseline is effectively zero, leave cell empty to avoid blow-ups.
            """
            if not np.isfinite(m_val) or not np.isfinite(b_val):
                return ""
            eps = 1e-12
            if abs(b_val) < eps:
                return ""
            pct = (float(m_val) - float(b_val)) / float(b_val) * 100.0
            return f"{pct:+.1f}%"

        # Collect flat diff records for printing and CSV
        flat_diffs: List[Tuple[str, str, float, float, float]] = []  # (method, base, pct, m_val, b_val)
        for m in comp_methods:
            m_val = med_series.get(m, np.nan)
            if not np.isfinite(m_val):
                continue
            row = [_short_method(m)]
            for b in baseline_cols:
                # Only show if that baseline is requested by this method
                if b in (cmp_map.get(m, []) or []) and b in med_series.index:
                    b_val = med_series.get(b, np.nan)
                    text = _compute_cell(float(m_val), float(b_val))
                    row.append(text)
                    # also store flat diff if valid
                    try:
                        if text:
                            pct = float(text.replace('%',''))
                            flat_diffs.append((m, b, pct, float(m_val), float(b_val)))
                    except Exception:
                        pass
                else:
                    row.append("")
            rows.append(row)

        # Print diffs (Rich) as a flat table
        if not quiet:
            try:
                if flat_diffs:
                    if _RICH_AVAILABLE:
                        dtbl = Table(title=f"Diffs — {metric} (Δ% = 100·(m − b)/b)", show_lines=False, header_style="bold magenta")
                        dtbl.add_column("#", justify="right", style="dim")
                        dtbl.add_column("Method")
                        dtbl.add_column("Baseline")
                        dtbl.add_column("Δ%", justify="right")
                        for idx, (m_name, b_name, pct, _mv, _bv) in enumerate(flat_diffs, start=1):
                            dtbl.add_row(str(idx), m_name, b_name, f"{pct:+.1f}%")
                        _console.print(dtbl)
                    else:
                        print(f"\nDiffs — {metric} (Δ% = 100·(m − b)/b)")
                        for idx, (m_name, b_name, pct, _mv, _bv) in enumerate(flat_diffs, start=1):
                            print(f"  {idx:>2}. {m_name} vs {b_name}: {pct:+.1f}%")
            except Exception:
                pass

        # Accumulate for CSV export
        for (m_name, b_name, pct, m_val, b_val) in flat_diffs:
            self._diff_records.append({
                "Metric": metric,
                "Method": m_name,
                "Baseline": b_name,
                "MeasuredMedian": m_val,
                "BaselineMedian": b_val,
                "DeltaPercent": pct,
            })

        # Render as a compact table
        ax_cmp.clear()
        ax_cmp.axis('off')
        if rows and baseline_cols:
            col_labels = ["Method"] + baseline_cols
            tbl = ax_cmp.table(cellText=rows, colLabels=col_labels, loc='center', cellLoc='center', colLoc='center')
            # Table aesthetics: make it larger and readable
            tbl.auto_set_font_size(False)
            tbl.set_fontsize(9)
            try:
                for (r, c), cell in tbl.get_celld().items():
                    cell.PAD = 0.06
                    cell.set_linewidth(0.4)
            except Exception:
                pass
            # Slightly scale up to make the table larger
            tbl.scale(1.08, 1.05)
            # Short caption below table with a single formula
            caption = "Δ% = 100·(m − b)/b"
            ax_cmp.text(0.5, 0.02, caption, transform=ax_cmp.transAxes, ha='center', va='bottom', fontsize=8)
        else:
            ax_cmp.text(0.5, 0.5, 'No compareOver baselines', transform=ax_cmp.transAxes, ha='center', va='center', fontsize=9)

        # Parameters text above comparison subplot
        if self._first_run_parameters:
            try:
                lines = [f"{k}: {self._first_run_parameters[k]}" for k in sorted(self._first_run_parameters.keys())]
                params_text = "\n".join(lines) if lines else "No parameters provided"
                # Move parameters box a bit below the top inside the axes
                ax_cmp.text(0.5, 0.98, params_text, transform=ax_cmp.transAxes, va='top', ha='center', fontsize=8,
                            bbox=dict(boxstyle='round', facecolor='white', alpha=0.9, edgecolor='lightgray'))
            except Exception:
                pass

        plt.tight_layout()
        fname = f"{BenchmarkPlotter._sanitizeFileName(metric)}.png"
        fpath = os.path.join(out_dir, fname)
        fig.savefig(fpath, bbox_inches="tight")
        if self.show:
            try:
                plt.show()
            except Exception:
                pass
        else:
            plt.close(fig)
        return fpath

    def plotDirectory(self) -> Dict[str, Any]:
        json_files = self._findJsonFiles(self.input_dir)
        if len(json_files) == 0:
            raise FileNotFoundError(f"No JSON files found in directory: {self.input_dir}")

        aggregated_records: List[pd.DataFrame] = []
        combined_plots: List[str] = []

        def _is_benchmark_json(data: Dict[str, Any]) -> bool:
            # Accept either new schema (set->{measurement:[values]}) or legacy (set->[values])
            ms = data.get("_measurementSets") or {}
            sw = data.get("_stopwatches") or {}
            has_ms = False
            if isinstance(ms, dict):
                for v in ms.values():
                    # Accept either flat list values, or nested {values: [...]} objects
                    if (isinstance(v, dict) and any((isinstance(lst, list) and len(lst) > 0) or (isinstance(lst, dict) and isinstance(lst.get("values", []), list) and len(lst.get("values", [])) > 0) for lst in v.values())) \
                       or (isinstance(v, list) and len(v) > 0):
                        has_ms = True
                        break
            has_sw = isinstance(sw, dict) and any(isinstance(v, list) and len(v) > 0 for v in sw.values())
            return has_ms or has_sw

        # Prepare deferred notices for clean output
        skipped_non_benchmarks: List[str] = []
        empty_runs: List[str] = []

        # Helper: process a single JSON file path, update state and return a DataFrame of records or None
        def process_one(path: str):
            run_label = self._labelFromFilename(path)
            data = self._loadJson(path)
            # benchmark JSON check
            def _is_benchmark_json(data: Dict[str, Any]) -> bool:
                ms = data.get("_measurementSets") or {}
                sw = data.get("_stopwatches") or {}
                has_ms = False
                if isinstance(ms, dict):
                    for v in ms.values():
                        if (isinstance(v, dict) and any((isinstance(lst, list) and len(lst) > 0) or (isinstance(lst, dict) and isinstance(lst.get("values", []), list) and len(lst.get("values", [])) > 0) for lst in v.values())) \
                           or (isinstance(v, list) and len(v) > 0):
                            has_ms = True
                            break
                has_sw = isinstance(sw, dict) and any(isinstance(v, list) and len(v) > 0 for v in sw.values())
                return has_ms or has_sw

            if not _is_benchmark_json(data):
                skipped_non_benchmarks.append(path)
                return None

            # Parse run metadata
            meta = self._parse_run_label(run_label)
            # Prepare per-run output directory
            per_run_dir = os.path.join(self.out_dir, run_label)
            os.makedirs(per_run_dir, exist_ok=True)
            # Build per-run stats
            plotter = BenchmarkPlotter(data, out_dir=self.out_dir, dpi=self.dpi, run_label=run_label, err=self.err, err_scale=self.err_scale, log_y=self.log_y)
            # Fallback populate meta from JSON parameters if missing
            try:
                params = plotter.parameters or {}
                if meta.get("unroll") is None:
                    ur = params.get("Unroll") or params.get("unroll")
                    if ur is not None:
                        try:
                            meta["unroll"] = int(float(str(ur)))
                        except Exception:
                            pass
                if meta.get("size") is None:
                    sz = params.get("Size") or params.get("size")
                    if sz is not None:
                        try:
                            meta["size"] = int(float(str(sz)))
                        except Exception:
                            pass
            except Exception:
                pass
            # Store run meta and parameters
            self._run_meta[run_label] = meta
            if not self._first_run_parameters and isinstance(plotter.parameters, dict):
                self._first_run_parameters = dict(plotter.parameters)
            # Merge compareOver maps
            for mt, mp in (plotter.compare_over or {}).items():
                tgt = self._compare_over.setdefault(mt, {})
                for meth, lst in (mp or {}).items():
                    if meth not in tgt:
                        tgt[meth] = list(lst)
            if getattr(plotter, '_cmp_sw_extracted', None):
                tgt_sw = self._compare_over.setdefault("Stopwatches", {})
                for meth, lst in (plotter._cmp_sw_extracted or {}).items():
                    if meth not in tgt_sw:
                        tgt_sw[meth] = list(lst)
            # Track first-seen metric/method order
            for metric in (plotter.measurement_sets or {}).keys():
                if metric not in self._metric_order:
                    self._metric_order.append(metric)
                methods_keys = list((plotter.measurement_sets or {}).get(metric, {}).keys())
                if methods_keys:
                    lst = self._method_order.setdefault(metric, [])
                    for m in methods_keys:
                        if m not in lst:
                            lst.append(m)
            if plotter.stopwatches:
                if "Stopwatches" not in self._metric_order:
                    self._metric_order.append("Stopwatches")
                sw_methods = list(plotter.stopwatches.keys())
                if sw_methods:
                    lst = self._method_order.setdefault("Stopwatches", [])
                    for m in sw_methods:
                        if m not in lst:
                            lst.append(m)
            # Merge raw values for aggregated errors
            for metric, methods in (plotter.measurement_sets or {}).items():
                tgt = self._agg_values.setdefault(metric, {})
                for method, vals in (methods or {}).items():
                    if not isinstance(vals, (list, tuple)):
                        continue
                    lst = tgt.setdefault(method, [])
                    lst.extend([float(x) for x in vals])
            if plotter.stopwatches:
                tgt_sw = self._agg_values.setdefault("Stopwatches", {})
                for method, vals in (plotter.stopwatches or {}).items():
                    if not isinstance(vals, (list, tuple)):
                        continue
                    lst = tgt_sw.setdefault(method, [])
                    lst.extend([float(x) for x in vals])
            # Compute stats to records
            metric_results, sw_result = plotter._computeAllStats()
            records: List[Dict[str, Any]] = []
            for res in metric_results + ([sw_result] if sw_result.method_stats else []):
                for method, stats in res.method_stats.items():
                    records.append({
                        "Run": run_label,
                        "Metric": res.metric_name,
                        "Method": method,
                        "N": stats.count,
                        "Median": stats.median,
                        "Mean": stats.mean,
                        "Std": stats.std,
                        "Min": stats.min,
                        "Max": stats.max,
                    })
            if not records:
                empty_runs.append(f"{run_label} ← {path}")
                return None
            # Per-run plots with this run's parameters and raw-only yerr
            try:
                per_df = pd.DataFrame.from_records(records)
                old_params = dict(self._first_run_parameters)
                self._first_run_parameters = dict(plotter.parameters or {})
                old_agg = self._agg_values
                try:
                    temp_agg: Dict[str, Dict[str, List[float]]] = {}
                    for met_name, methods in (plotter.measurement_sets or {}).items():
                        temp_agg[met_name] = {m: list(vals) for m, vals in (methods or {}).items() if isinstance(vals, (list, tuple))}
                    if plotter.stopwatches:
                        temp_agg["Stopwatches"] = {m: list(vals) for m, vals in (plotter.stopwatches or {}).items() if isinstance(vals, (list, tuple))}
                    self._agg_values = temp_agg
                    present_metrics = sorted(per_df["Metric"].unique().tolist())
                    for met in present_metrics:
                        try:
                            self._plotMedianByMethod(per_df, met, per_run_dir, quiet=True)
                        except Exception:
                            pass
                finally:
                    self._agg_values = old_agg
                    self._first_run_parameters = old_params
            except Exception:
                pass
            return pd.DataFrame.from_records(records)

        # Use a proper Rich progress bar with in-place updates; no printing inside the loop
        use_progress = _RICH_AVAILABLE
        if use_progress:
            try:
                from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn, TaskProgressColumn, TimeElapsedColumn, TimeRemainingColumn
                progress = Progress(
                    SpinnerColumn(style="cyan"),
                    TextColumn("[progress.description]{task.description}"),
                    BarColumn(bar_width=None),
                    TaskProgressColumn(),
                    TextColumn("•"),
                    TimeElapsedColumn(),
                    TextColumn("•"),
                    TimeRemainingColumn(),
                    transient=False,
                )
            except Exception:
                use_progress = False

        if use_progress:
            try:
                with progress as prog:
                    task_id = prog.add_task("Parsing JSON files", total=len(json_files))
                    for path in json_files:
                        try:
                            prog.update(task_id, description=f"Parsing: {os.path.basename(path)}")
                        except Exception:
                            pass
                        df = process_one(path)
                        if df is not None:
                            aggregated_records.append(df)
                        try:
                            prog.advance(task_id)
                        except Exception:
                            pass
            except Exception:
                # Fallback to simple loop if progress bar initialization fails
                use_progress = False

        if not use_progress:
            for path in json_files:
                df = process_one(path)
                if df is not None:
                    aggregated_records.append(df)

        # After parsing, print any deferred notices
        try:
            if skipped_non_benchmarks:
                if _RICH_AVAILABLE:
                    tbl = Table(title="Skipped non-benchmark JSONs", show_lines=False, header_style="bold yellow")
                    tbl.add_column("#", justify="right", style="dim")
                    tbl.add_column("Path", overflow="fold")
                    for idx, p in enumerate(skipped_non_benchmarks, start=1):
                        tbl.add_row(str(idx), p)
                    _console.print(tbl)
                else:
                    print("\nSkipped non-benchmark JSONs:")
                    for p in skipped_non_benchmarks:
                        print(" -", p)
            if empty_runs:
                if _RICH_AVAILABLE:
                    tbl = Table(title="Runs with no extractable metrics", show_lines=False, header_style="bold yellow")
                    tbl.add_column("#", justify="right", style="dim")
                    tbl.add_column("Run")
                    for idx, r in enumerate(empty_runs, start=1):
                        tbl.add_row(str(idx), r)
                    _console.print(tbl)
                else:
                    print("\nRuns with no extractable metrics:")
                    for r in empty_runs:
                        print(" -", r)
        except Exception:
            pass

        # Aggregate summaries
        if len(aggregated_records) > 0:
            agg_df = pd.concat(aggregated_records, ignore_index=True)
            # Ensure required columns exist
            expected_cols = ["Run", "Metric", "Method", "N", "Median", "Mean", "Std", "Min", "Max"]
            missing = [c for c in expected_cols if c not in agg_df.columns]
            if missing:
                raise KeyError(f"Aggregated DataFrame missing required columns: {missing}")
            agg_df = agg_df[expected_cols]
            agg_df = agg_df.sort_values(["Metric", "Method", "Run"])
            agg_csv = os.path.join(self.out_dir, "ALL_runs_summary_stats.csv")
            agg_df.to_csv(agg_csv, index=False)
        else:
            # Create an empty CSV to signal that nothing was found
            agg_df = pd.DataFrame(columns=["Run", "Metric", "Method", "N", "Median", "Mean", "Std", "Min", "Max"])
            agg_csv = os.path.join(self.out_dir, "ALL_runs_summary_stats.csv")
            agg_df.to_csv(agg_csv, index=False)

    # Aggregated median across runs per metric
        if not agg_df.empty:
            present = set(agg_df["Metric"].unique().tolist())
            if self._metric_order:
                metrics = [mt for mt in self._metric_order if mt in present]
            else:
                metrics = list(dict.fromkeys(agg_df["Metric"].tolist()))
        else:
            metrics = []
        for metric in metrics:
            # Skip metrics that didn't produce any data (e.g., _plot=false)
            if metric not in self._agg_values or not self._agg_values.get(metric):
                continue
            plot_path = self._plotMedianByMethod(agg_df, metric, self.out_dir)
            if plot_path:
                combined_plots.append(plot_path)

        # Build scaling plots per operation and per metric: now partitioned by dtype and sizeType,
        # with all unrolls for Stalker and only one curve for other families.
        self._scaling_plots = []
        if not agg_df.empty and self._run_meta:
            # Attach Size, Operation, Unroll, DType, SizeType to agg_df rows
            def _meta_col(run, key):
                m = self._run_meta.get(run, {})
                return m.get(key)
            agg_df = agg_df.copy()
            agg_df["Size"] = agg_df["Run"].map(lambda r: _meta_col(r, "size"))
            agg_df["Operation"] = agg_df["Run"].map(lambda r: _meta_col(r, "operation"))
            agg_df["Unroll"] = agg_df["Run"].map(lambda r: _meta_col(r, "unroll"))
            agg_df["DType"] = agg_df["Run"].map(lambda r: _meta_col(r, "dtype"))
            agg_df["SizeType"] = agg_df["Run"].map(lambda r: _meta_col(r, "sizeType"))
            agg_df = agg_df[pd.notna(agg_df["Size"]) & pd.notna(agg_df["Operation"])].copy()
            if not agg_df.empty:
                for op in sorted(agg_df["Operation"].dropna().unique().tolist()):
                    op_base_df = agg_df[agg_df["Operation"] == op]
                    if op_base_df.empty:
                        continue
                    op_out = os.path.join(self.out_dir, f"scaling_{op}")
                    os.makedirs(op_out, exist_ok=True)
                    for dtype in sorted([x for x in op_base_df["DType"].dropna().unique().tolist()]):
                        dtype_df = op_base_df[op_base_df["DType"] == dtype]
                        if dtype_df.empty:
                            continue
                        present_groups = set([str(x) for x in dtype_df["SizeType"].dropna().unique().tolist()])
                        groups_to_render: List[str] = []
                        if present_groups - {"ND"}:
                            if "S" in present_groups:
                                groups_to_render.append("S")
                            if "L" in present_groups:
                                groups_to_render.append("L")
                        elif "ND" in present_groups:
                            groups_to_render = ["ND"]
                        for grp in groups_to_render:
                            grp_df = dtype_df[dtype_df["SizeType"] == grp]
                            if grp_df.empty:
                                continue
                            for metric in sorted(grp_df["Metric"].unique().tolist()):
                                mdf = grp_df[grp_df["Metric"] == metric]
                                if mdf.empty:
                                    continue
                                # Candidate datasets by (method, unroll)
                                all_pairs = sorted(set((row["Method"], row["Unroll"]) for _, row in mdf[["Method", "Unroll"]].drop_duplicates().iterrows()), key=lambda x: (str(x[0]), x[1] if x[1] is not None else -1))
                                if not all_pairs:
                                    continue
                                family_marker = {
                                    'avx2': 'x',
                                    'avx512': 'o',
                                    'std': 's',
                                    'blas': '^',
                                    'eigen': 'D',
                                    'other': 'v',
                                }

                                def _family_from_method(m: str) -> str:
                                    s = (m or '').strip().lower()
                                    if s.startswith('stalker avx2'):
                                        return 'avx2'
                                    if s.startswith('stalker avx512'):
                                        return 'avx512'
                                    if s.startswith('std::'):
                                        return 'std'
                                    if s.startswith('blas'):
                                        return 'blas'
                                    if s.startswith('eigen'):
                                        return 'eigen'
                                    return 'other'

                                datasets: List[Tuple[str, Any]] = []
                                unrolls_by_method: Dict[str, List[Any]] = {}
                                for method, unroll in all_pairs:
                                    unrolls_by_method.setdefault(method, []).append(unroll)
                                for method, unrolls in unrolls_by_method.items():
                                    fam = _family_from_method(method)
                                    if fam in ("avx2", "avx512"):
                                        for u in sorted(unrolls, key=lambda x: (x is None, x if x is not None else -1)):
                                            datasets.append((method, u))
                                    else:
                                        pick = None
                                        if 1 in unrolls:
                                            pick = 1
                                        else:
                                            nums = [u for u in unrolls if isinstance(u, (int, float))]
                                            if nums:
                                                pick = int(sorted(nums)[0])
                                        datasets.append((method, pick))
                                fig, ax = plt.subplots(figsize=(8.8, 5.2), dpi=self.dpi)
                                for idx, (method, unroll) in enumerate(datasets):
                                    if unroll is None or (isinstance(unroll, float) and math.isnan(unroll)):
                                        ddf = mdf[(mdf["Method"] == method) & (mdf["Unroll"].isna())]
                                    else:
                                        ddf = mdf[(mdf["Method"] == method) & (mdf["Unroll"] == unroll)]
                                    if pd.isna(ddf).all().any():
                                        ddf = ddf.dropna()
                                    by_size = ddf.groupby("Size")["Median"].median().sort_index()
                                    sizes = by_size.index.to_list()
                                    values = by_size.values.tolist()

                                    fam = _family_from_method(method)
                                    mrk = family_marker.get(fam, family_marker['other'])
                                    if fam in ("avx2", "avx512"):
                                        lbl = f"{method} (u{unroll if unroll is not None else '?'})"
                                    else:
                                        lbl = f"{method}"
                                    ax.plot(
                                        sizes,
                                        values,
                                        marker=mrk,
                                        linewidth=0.9,
                                        markersize=5.0,
                                        markerfacecolor='none',
                                        markeredgewidth=1.0,
                                        label=lbl,
                                    )
                                title_dtype = {"d": "double", "f": "float"}.get(dtype, str(dtype))
                                grp_name = {"S": "Small", "L": "Large", "ND": "All"}.get(grp, grp)
                                ax.set_title(f"{op} — {metric} vs Size — {title_dtype} — {grp_name}")
                                ax.set_xlabel("Size")
                                ax.set_ylabel(metric)
                                ax.grid(True, which='both', axis='both', linestyle=':', linewidth=0.8, alpha=0.9)
                                try:
                                    ax.xaxis.set_major_locator(mticker.MaxNLocator(nbins=8, min_n_ticks=4, integer=True))
                                except Exception:
                                    pass
                                ax.legend(fontsize=9, loc='best', frameon=True)
                                plt.tight_layout()
                                fname = f"{op}_{BenchmarkPlotter._sanitizeFileName(metric)}_{title_dtype}_{grp}_scaling.png"
                                fpath = os.path.join(op_out, fname)
                                fig.savefig(fpath, bbox_inches="tight")
                                plt.close(fig)
                                self._scaling_plots.append(fpath)

        # Write diffs CSV if any
        diffs_csv = None
        try:
            if self._diff_records:
                diffs_df = pd.DataFrame.from_records(self._diff_records)
                # Order columns for readability
                diffs_cols = ["Metric", "Method", "Baseline", "MeasuredMedian", "BaselineMedian", "DeltaPercent"]
                diffs_df = diffs_df[diffs_cols]
                diffs_csv = os.path.join(self.out_dir, "ALL_runs_differences.csv")
                diffs_df.to_csv(diffs_csv, index=False)
        except Exception:
            diffs_csv = None

        return {
            "combined_plots": combined_plots,
            "scaling_plots": list(self._scaling_plots),
            "all_runs_summary_csv": agg_csv,
            "all_runs_diffs_csv": diffs_csv,
            "json_files": json_files,
        }


# ---------------------------- CLI ----------------------------

def main():
    parser = argparse.ArgumentParser(description="HPC Benchmark Plotter (multi-file)")
    parser.add_argument("--input_dir", "--in", dest="input_dir", required=True, help="Directory containing one or more JSON files")
    parser.add_argument("--out", default=None, help="Output base directory (optional). If omitted, input_dir is used. A timestamped subdir will be created inside.")
    parser.add_argument("--dpi", type=int, default=140, help="Figure DPI")
    parser.add_argument("--show", action="store_true", help="Display plots after saving")
    parser.add_argument("--err", choices=["std", "stderr", "none"], default="std", help="Error bar mode: standard deviation (std), standard error (stderr), or none")
    parser.add_argument("--err-scale", type=float, default=1.0, help="Scale factor applied to error bars (e.g., 0.5 to shrink, 2.0 to grow)")
    parser.add_argument("--log-y", action="store_true", help="Use logarithmic y-axis")
    args = parser.parse_args()

    # Create base output directory: use --out if provided, else the input_dir
    base_out = args.out if args.out else args.input_dir
    os.makedirs(base_out, exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    out_dir = os.path.join(base_out, timestamp)
    os.makedirs(out_dir, exist_ok=True)

    mbp = MultiBenchmarkPlotter(input_dir=args.input_dir, out_dir=out_dir, dpi=args.dpi, show=args.show, err=args.err, err_scale=args.err_scale, log_y=args.log_y)
    results = mbp.plotDirectory()

    if _RICH_AVAILABLE:
        _console.rule("Benchmark Plot Summary")
        # Files table
        files_tbl = Table(title="Parsed JSON files", show_lines=False, header_style="bold cyan")
        files_tbl.add_column("#", justify="right", style="dim")
        files_tbl.add_column("Path", overflow="fold")
        for idx, p in enumerate(results["json_files"], start=1):
            files_tbl.add_row(str(idx), p)
        _console.print(files_tbl)

        # Outputs table
        out_tbl = Table(title="Outputs", show_lines=False, header_style="bold green")
        out_tbl.add_column("Item", style="bold")
        out_tbl.add_column("Value", overflow="fold")
        out_tbl.add_row("Output directory", out_dir)
        out_tbl.add_row("All-runs CSV", results["all_runs_summary_csv"])
        if results.get("all_runs_diffs_csv"):
            out_tbl.add_row("All-diffs CSV", results["all_runs_diffs_csv"])
        out_tbl.add_row("Error mode", getattr(mbp, 'err', 'std'))
        out_tbl.add_row("Error scale", f"{getattr(mbp, 'err_scale', 1.0)}")
        out_tbl.add_row("Log Y", "yes" if getattr(mbp, 'log_y', False) else "no")
        _console.print(out_tbl)

        if results["combined_plots"]:
            plots_tbl = Table(title="Generated Plots", show_lines=False, header_style="bold magenta")
            plots_tbl.add_column("#", justify="right", style="dim")
            plots_tbl.add_column("Path", overflow="fold")
            for idx, p in enumerate(results["combined_plots"], start=1):
                plots_tbl.add_row(str(idx), p)
            _console.print(plots_tbl)
        else:
            _console.print(Panel("No plots generated.", title="Plots", title_align="left"))

        # Scaling plots table
        if results.get("scaling_plots"):
            s_tbl = Table(title="Scaling Plots", show_lines=False, header_style="bold yellow")
            s_tbl.add_column("#", justify="right", style="dim")
            s_tbl.add_column("Path", overflow="fold")
            for idx, p in enumerate(results["scaling_plots"], start=1):
                s_tbl.add_row(str(idx), p)
            _console.print(s_tbl)
    else:
        print("Parsed JSON files:")
        for p in results["json_files"]:
            print(" ", p)

        print("\nAggregated median plots saved under:", out_dir)

        print("\nPlots:")
        for p in results["combined_plots"]:
            print(" ", p)

        print("\nAll-runs CSV:", results["all_runs_summary_csv"])
        if results.get("all_runs_diffs_csv"):
            print("All-diffs CSV:", results["all_runs_diffs_csv"]) 
        if results.get("scaling_plots"):
            print("\nScaling plots:")
            for p in results["scaling_plots"]:
                print(" ", p)


if __name__ == "__main__":
    main()
