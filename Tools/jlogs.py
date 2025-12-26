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
    def __init__(self, data: Dict[str, Any], out_dir: str, dpi: int = 140, run_label: str = "", err: str = "std", err_scale: float = 1.0):
        self.data = data
        self.out_dir = out_dir
        self.dpi = dpi
        self.run_label = run_label
        self.err = err  # 'std' | 'stderr' | 'none'
        self.err_scale = err_scale
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
            # Normalize metric name: gbps -> GB/s
            if "gbps" in metric.lower():
                metric = metric.replace("gbps", "GB/s").replace("Gbps", "GB/s")
            
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
        sw_res = MetricSetResult(metric_name="Time")
        for method, values in self.stopwatches.items():
            sw_res.method_stats[method] = self._computeStats(values)
        return results, sw_res

    @staticmethod
    def _sanitizeFileName(name: str) -> str:
        return "".join(ch if ch.isalnum() or ch in (" ", "_", "-") else "_" for ch in name).replace(" ", "_")


# ---------------------------- Multi-run Orchestrator ----------------------------

class MultiBenchmarkPlotter:
    def __init__(self, input_dir: str, out_dir: str = "plots", dpi: int = 140, show: bool = False, err: str = "std", err_scale: float = 1.0):
        self.input_dir = input_dir
        self.out_dir = out_dir
        self.dpi = dpi
        self.show = show
        self.err = err
        self.err_scale = err_scale
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

    def _plotMedianByMethod(self, combined_df: pd.DataFrame, metric: str, out_dir: str, quiet: bool = False, custom_filename: str = None) -> str:
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
        
        # Force scientific notation
        try:
            ax.ticklabel_format(axis='y', style='sci', scilimits=(0,0))
        except Exception:
            pass

        # Grid consistency
        ax.grid(True, which='major', axis='y', linestyle='-', linewidth=0.8, alpha=0.8)
        ax.grid(True, which='minor', axis='y', linestyle=':', linewidth=0.5, alpha=0.5)

        # Finer y-axis ticks and minor grid
        try:
            ax.yaxis.set_major_locator(mticker.MaxNLocator(nbins=10, min_n_ticks=6))
            ax.yaxis.set_minor_locator(mticker.AutoMinorLocator(2))
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
        fname = custom_filename if custom_filename else f"{BenchmarkPlotter._sanitizeFileName(metric)}.png"
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
            # Strip timestamp suffix if present (e.g. _23122025_004108_...)
            # Regex: look for _DDMMYYYY_HHMMSS...
            import re
            # Matches _DDMMYYYY_HHMMSS_... at end of string
            run_label_clean = re.sub(r'_\d{8}_\d{6}_.*$', '', run_label)
            
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
            # Build per-run stats
            plotter = BenchmarkPlotter(data, out_dir=self.out_dir, dpi=self.dpi, run_label=run_label, err=self.err, err_scale=self.err_scale)
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
            # Store run meta and parameters (using clean label handled later)
            # self._run_meta[run_label] = meta  <-- Moved to end of function using clean label
            if not self._first_run_parameters and isinstance(plotter.parameters, dict):
                self._first_run_parameters = dict(plotter.parameters)
            # Merge compareOver maps
            for mt, mp in (plotter.compare_over or {}).items():
                tgt = self._compare_over.setdefault(mt, {})
                for meth, lst in (mp or {}).items():
                    if meth not in tgt:
                        tgt[meth] = list(lst)
            if getattr(plotter, '_cmp_sw_extracted', None):
                tgt_sw = self._compare_over.setdefault("Time", {})
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
                if "Time" not in self._metric_order:
                    self._metric_order.append("Time")
                sw_methods = list(plotter.stopwatches.keys())
                if sw_methods:
                    lst = self._method_order.setdefault("Time", [])
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
                tgt_sw = self._agg_values.setdefault("Time", {})
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
                        "Run": run_label_clean, # Use clean label for aggregation
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
            
            # Store metadata using the clean label as key
            self._run_meta[run_label_clean] = meta
            
            # Per-run plots are now disabled in favor of aggregated size-based plots
            # We return the records to be aggregated later
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

    # Aggregated median across runs per metric - DISABLED as requested
    #    if not agg_df.empty:
    #        present = set(agg_df["Metric"].unique().tolist())
    #        if self._metric_order:
    #            metrics = [mt for mt in self._metric_order if mt in present]
    #        else:
    #            metrics = list(dict.fromkeys(agg_df["Metric"].tolist()))
    #    else:
    #        metrics = []
    #    for metric in metrics:
    #        # Skip metrics that didn't produce any data (e.g., _plot=false)
    #        if metric not in self._agg_values or not self._agg_values.get(metric):
    #            continue
    #        plot_path = self._plotMedianByMethod(agg_df, metric, self.out_dir)
    #        if plot_path:
    #            combined_plots.append(plot_path)

        # Build grouped bar plots per (Operation, Size, DType)
        # This replaces the individual per-run plots with a single plot comparing all unrolls/methods for a specific size.
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
            
            # Filter valid rows
            valid_df = agg_df[pd.notna(agg_df["Size"]) & pd.notna(agg_df["Operation"])].copy()
            
            if not valid_df.empty:
                # 1. Grouped Bar Plots (Operation + Size + DType)
                # Iterate over unique combinations of Operation, Size, DType
                unique_groups = valid_df[["Operation", "Size", "DType"]].drop_duplicates()
                
                for _, group_row in unique_groups.iterrows():
                    op = group_row["Operation"]
                    sz = group_row["Size"]
                    dt = group_row["DType"]
                    
                    # Filter data for this specific chart
                    chart_df = valid_df[
                        (valid_df["Operation"] == op) & 
                        (valid_df["Size"] == sz) & 
                        (valid_df["DType"] == dt)
                    ].copy()
                    
                    if chart_df.empty:
                        continue

                    # Deduplication and Labeling Logic
                    def _get_canonical_method(row):
                        m = (row["Method"] or "").strip()
                        u = row["Unroll"]
                        m_lower = m.lower()
                        
                        # External libraries: ignore unroll, merge all
                        if m_lower.startswith("blas") or m_lower.startswith("eigen") or m_lower.startswith("std"):
                            return m # Just the method name
                        
                        # Stalker: keep unroll
                        if u is not None and not pd.isna(u):
                            return f"{m} (u{int(u)})"
                        return m

                    chart_df["CanonicalMethod"] = chart_df.apply(_get_canonical_method, axis=1)
                    
                    # Group by CanonicalMethod to merge duplicates (e.g. blas u1, blas u4 -> blas)
                    # We take the median of medians
                    grouped_chart = chart_df.groupby(["Metric", "CanonicalMethod"]).agg({
                        "Median": "median",
                        "Std": "mean" # Approximate error propagation
                    }).reset_index()

                    # Color mapping
                    def _get_color(method_label):
                        m = method_label.lower()
                        if "stalker" in m: return "#FFC300" # Van Gogh Orange
                        if "eigen" in m: return "#274690" # Ultramarine Blue
                        if "blas" in m: return "#D1495B" # Reddish
                        if "std" in m: return "#4C9A2A" # Green
                        return "#6C757D" # Gray
                    
                    # Iterate over metrics available for this slice
                    for metric in grouped_chart["Metric"].unique():
                        m_df = grouped_chart[grouped_chart["Metric"] == metric].copy()
                        if m_df.empty:
                            continue
                            
                        # Prepare output directory: OUT_DIR/Operation/Metric/
                        safe_met = BenchmarkPlotter._sanitizeFileName(metric)
                        safe_op = BenchmarkPlotter._sanitizeFileName(op)
                        target_dir = os.path.join(self.out_dir, safe_op, safe_met)
                        os.makedirs(target_dir, exist_ok=True)
                        
                        # Filename: Size_DType.png
                        fname = f"Size{int(sz)}_{dt}.png"
                        
                        # Plotting
                        m_df = m_df.sort_values("CanonicalMethod")
                        
                        fig, ax = plt.subplots(figsize=(8, 8), dpi=self.dpi) # Square plot
                        
                        methods = m_df["CanonicalMethod"].tolist()
                        medians = m_df["Median"].tolist()
                        yerrs = m_df["Std"].tolist() if self.err == "std" else None
                        colors = [_get_color(m) for m in methods]
                        
                        x_pos = np.arange(len(methods))
                        bars = ax.bar(x_pos, medians, yerr=yerrs, capsize=4, alpha=0.9, color=colors)
                        
                        ax.set_xticks(x_pos)
                        ax.set_xticklabels(methods, rotation=45, ha="right")
                        ax.set_title(f"{op} (Size={int(sz)}, {dt}) — {metric}")
                        ax.set_ylabel(metric)
                        ax.grid(True, axis='y', linestyle='--', alpha=0.7)
                        
                        # Add value labels on top of bars
                        for bar in bars:
                            height = bar.get_height()
                            ax.annotate(f'{height:.2e}',
                                        xy=(bar.get_x() + bar.get_width() / 2, height),
                                        xytext=(0, 3),  # 3 points vertical offset
                                        textcoords="offset points",
                                        ha='center', va='bottom', fontsize=8, rotation=0)

                        plt.tight_layout()
                        fpath = os.path.join(target_dir, fname)
                        fig.savefig(fpath)
                        plt.close(fig)

            # 2. Scaling Plots (Line charts across sizes)
            agg_df = valid_df # Use the prepared valid_df for scaling plots too
            if not agg_df.empty:
                for op in sorted(agg_df["Operation"].dropna().unique().tolist()):
                    op_base_df = agg_df[agg_df["Operation"] == op]
                    if op_base_df.empty:
                        continue
                    safe_op = BenchmarkPlotter._sanitizeFileName(op)
                    op_out = os.path.join(self.out_dir, safe_op, "scaling")
                    os.makedirs(op_out, exist_ok=True)
                    for dtype in sorted([x for x in op_base_df["DType"].dropna().unique().tolist()]):
                        dtype_df = op_base_df[op_base_df["DType"] == dtype]
                        if dtype_df.empty:
                            continue
                        present_groups = set([str(x) for x in dtype_df["SizeType"].dropna().unique().tolist()])
                        groups_to_render: List[str] = []
                        # Only render the combined "All sizes" (ND) plot
                        groups_to_render = ["ND"]
                        for grp in groups_to_render:
                            # For the 'ND' (All sizes) plot combine across all SizeType entries
                            grp_df = dtype_df.copy()
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
                                    'std': '^',
                                    'blas': '<',
                                    'eigen': '>',
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
                                        # For non-SIMD, we merge all unrolls into one dataset
                                        datasets.append((method, 'ALL'))
                                
                                # Generate plots: Linear-Linear, Log-Linear (Log X)
                                plot_configs = [
                                    ("LinLin", False, False),
                                    ("LogLin", True, False)
                                ]
                                for scale_label, use_log_x, use_log_y in plot_configs:
                                    fig, ax = plt.subplots(figsize=(8.0, 8.0), dpi=self.dpi)
                                    for idx, (method, unroll) in enumerate(datasets):
                                        if unroll == 'ALL':
                                            ddf = mdf[mdf["Method"] == method]
                                            lbl_unroll = "merged"
                                        elif unroll is None or (isinstance(unroll, float) and math.isnan(unroll)):
                                            ddf = mdf[(mdf["Method"] == method) & (mdf["Unroll"].isna())]
                                            lbl_unroll = "?"
                                        else:
                                            ddf = mdf[(mdf["Method"] == method) & (mdf["Unroll"] == unroll)]
                                            lbl_unroll = f"u{unroll}"
                                        
                                        # Robust dropna: only drop if Median or Size is missing
                                        ddf = ddf.dropna(subset=["Median", "Size"])
                                        
                                        if ddf.empty:
                                            continue

                                        by_size = ddf.groupby("Size")["Median"].median().sort_index()
                                        sizes = by_size.index.to_list()
                                        values = by_size.values.tolist()

                                        fam = _family_from_method(method)
                                        mrk = family_marker.get(fam, family_marker['other'])
                                        
                                        if fam in ("avx2", "avx512"):
                                            lbl = f"{method} ({lbl_unroll})"
                                        else:
                                            lbl = f"{method}"
                                            
                                        # Reduce marker density
                                        npoints = len(sizes)
                                        try:
                                            # Increase marker frequency (target ~12 markers per line)
                                            me = max(1, int(math.ceil(float(npoints) / 24)))
                                        except Exception:
                                            me = None
                                            
                                        ax.plot(
                                            sizes,
                                            values,
                                            marker=mrk,
                                            linewidth=0.8,
                                            markersize=5.0,
                                            markeredgewidth=0.8,
                                            markerfacecolor='none',
                                            markevery=me,
                                            label=lbl,
                                        )
                                    title_dtype = {"d": "double", "f": "float"}.get(dtype, str(dtype))
                                    grp_name = "All"
                                    ax.set_title(f"{op} — {metric} vs Size — {title_dtype} — {scale_label}")
                                    ax.set_xlabel("Size")
                                    ax.set_ylabel(metric)
                                    
                                    # Force scientific notation only for linear Y
                                    if not use_log_y:
                                        try:
                                            ax.ticklabel_format(axis='y', style='sci', scilimits=(0,0))
                                        except Exception:
                                            pass

                                    # Grid consistency
                                    ax.grid(True, which='major', linestyle='-', linewidth=0.8, alpha=0.8)
                                    ax.grid(True, which='minor', linestyle=':', linewidth=0.5, alpha=0.5)

                                    if use_log_x:
                                        try:
                                            ax.set_xscale('log')
                                        except Exception:
                                            pass
                                    else:
                                        try:
                                            ax.xaxis.set_major_locator(mticker.MaxNLocator(nbins=8, min_n_ticks=4, integer=True))
                                        except Exception:
                                            pass

                                    if use_log_y:
                                        try:
                                            ax.set_yscale('log')
                                        except Exception:
                                            pass

                                    ax.legend(fontsize=9, loc='best', frameon=True)
                                    plt.tight_layout()
                                    fname = f"{op}_{BenchmarkPlotter._sanitizeFileName(metric)}_{title_dtype}_{scale_label}_scaling.png"
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
    args = parser.parse_args()

    # Create base output directory: use --out if provided, else the input_dir
    base_out = args.out if args.out else args.input_dir
    os.makedirs(base_out, exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    out_dir = os.path.join(base_out, timestamp)
    os.makedirs(out_dir, exist_ok=True)

    mbp = MultiBenchmarkPlotter(input_dir=args.input_dir, out_dir=out_dir, dpi=args.dpi, show=args.show, err=args.err, err_scale=args.err_scale)
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
