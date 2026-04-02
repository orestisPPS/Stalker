"""C++ Benchmark log visualization page."""
import streamlit as st
import os
from ..engine.benchmark_loader import BenchmarkLoader
from ..components.charts import render_benchmark_chart


def render_benchmarks_page(logs_dir: str):
    st.header("C++ Benchmark Logs")

    loader = BenchmarkLoader(logs_dir)
    log_files = loader.list_logs()

    if not log_files:
        st.info(f"No benchmark JSON logs found in `{logs_dir}`.")
        return

    st.metric("Log files found", len(log_files))

    # Group by operation
    groups = loader.group_by_operation()
    tabs = st.tabs(list(groups.keys()))

    for tab, (op_name, logs) in zip(tabs, groups.items()):
        with tab:
            st.subheader(f"Operation: {op_name}")
            render_benchmark_chart(logs, title=op_name)

            with st.expander(f"Raw data — {op_name} ({len(logs)} log files)"):
                for log in logs:
                    st.markdown(f"**{log.filename}** — Size: {log.size}")
                    for unit, series in log.measurement_sets.items():
                        for name, s in series.items():
                            st.text(f"  {name} [{unit}]: mean={s.mean:.2f}, std={s.std:.2f}, median={s.median:.2f}")
