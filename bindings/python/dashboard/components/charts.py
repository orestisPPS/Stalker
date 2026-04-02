"""Plotly chart builders for timing, speedup, and benchmark data."""
import streamlit as st
import plotly.graph_objects as go
import plotly.express as px
import pandas as pd
from typing import List, Tuple
from ..models.results import SuiteResults
from ..engine.benchmark_loader import BenchmarkLog
from ..components.result_card import OP_FULL_NAMES


def _full_op(short: str) -> str:
    return OP_FULL_NAMES.get(short, short)


def _build_timing_rows(suites: Tuple[Tuple]) -> pd.DataFrame:
    """Build timing DataFrame from hashable suite tuples (for caching)."""
    rows = []
    for s in suites:
        for r in s.results:
            rows.append({
                "Suite": s.suite_name, "Config": s.config_name, "DType": s.dtype,
                "Op": _full_op(r.op),
                "Stalker (μs)": r.time_us, "NumPy (μs)": r.py_time_us,
            })
    return pd.DataFrame(rows) if rows else pd.DataFrame()


def render_timing_chart(suites: List[SuiteResults], title: str = "Kernel Timing"):
    """Bar chart: Stalker vs NumPy timing per operation, grouped by suite."""
    df = _build_timing_rows(tuple(suites))
    if df.empty:
        return

    agg = df.groupby(["Suite", "Op"])[["Stalker (μs)", "NumPy (μs)"]].mean().reset_index()

    for suite_name in agg["Suite"].unique():
        sdf = agg[agg["Suite"] == suite_name].sort_values("Op")
        fig = go.Figure()
        fig.add_trace(go.Bar(name="Stalker", x=sdf["Op"], y=sdf["Stalker (μs)"], marker_color="#00d4aa"))
        fig.add_trace(go.Bar(name="NumPy", x=sdf["Op"], y=sdf["NumPy (μs)"], marker_color="#ff6b6b"))
        fig.update_layout(
            title=f"{title} — {suite_name}", barmode="group",
            template="plotly_dark", height=400,
            xaxis_title="Operation", yaxis_title="Time (μs)",
            font=dict(size=12),
        )
        st.plotly_chart(fig, use_container_width=True)


def render_speedup_chart(suites: List[SuiteResults], title: str = "Speedup vs NumPy"):
    """Horizontal bar chart showing speedup ratio per operation, grouped by suite."""
    rows = []
    for s in suites:
        for r in s.results:
            if r.speedup and r.speedup > 0:
                rows.append({
                    "Suite": s.suite_name,
                    "Op": _full_op(r.op),
                    "Speedup": r.speedup,
                })

    if not rows:
        st.info("No speedup data available (NumPy timing missing).")
        return

    df = pd.DataFrame(rows)

    for suite_name in df["Suite"].unique():
        sdf = df[df["Suite"] == suite_name]
        agg = sdf.groupby("Op")["Speedup"].mean().reset_index().sort_values("Speedup", ascending=True)

        fig = go.Figure(go.Bar(
            x=agg["Speedup"], y=agg["Op"], orientation="h",
            marker_color=["#00d4aa" if v >= 1 else "#ff6b6b" for v in agg["Speedup"]],
            text=[f"{v:.1f}×" for v in agg["Speedup"]], textposition="auto",
        ))
        fig.add_vline(x=1.0, line_dash="dash", line_color="white", opacity=0.5)
        fig.update_layout(
            title=f"{title} — {suite_name}", template="plotly_dark",
            height=max(300, len(agg) * 30),
            xaxis_title="Speedup (×)", yaxis_title="",
            font=dict(size=12),
        )
        st.plotly_chart(fig, use_container_width=True)


def render_benchmark_chart(logs: List[BenchmarkLog], title: str = "C++ Benchmark Data"):
    """Visualize C++ benchmark logs — throughput vs size per kernel."""
    if not logs:
        st.info("No benchmark logs found.")
        return

    rows = []
    for log in logs:
        size = log.size
        if size is None:
            continue
        for unit, series in log.measurement_sets.items():
            for name, s in series.items():
                rows.append({
                    "Size": size, "Kernel": name, "Unit": unit,
                    "Mean": s.mean, "Std": s.std, "Median": s.median,
                })

    if not rows:
        return

    df = pd.DataFrame(rows)
    units = df["Unit"].unique()

    for unit in units:
        udf = df[df["Unit"] == unit]
        fig = px.line(
            udf, x="Size", y="Mean", color="Kernel",
            title=f"{title} — {unit}", log_x=True,
            template="plotly_dark", height=500,
        )
        fig.update_layout(
            xaxis_title="Vector Size", yaxis_title=unit,
            font=dict(size=12),
        )
        st.plotly_chart(fig, use_container_width=True)
