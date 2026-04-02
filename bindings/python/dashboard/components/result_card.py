"""Collapsible result cards and tabular views."""
import streamlit as st
import pandas as pd
import numpy as np
from ..models.results import SuiteResults, TestResult

# ── Operation name mapping ───────────────────────────────────────────
OP_FULL_NAMES = {
    "cpy": "Copy", "swp": "Swap", "set": "Set Value",
    "add": "Add", "add_sc": "Add (Scaled)", "axpy": "AXPY",
    "sub": "Subtract", "sub_sc": "Subtract (Scaled)",
    "mul": "Multiply", "mul_sc": "Multiply (Scaled)",
    "scl": "Scale", "scl_i": "Scale (In-place)",
    "add_c": "Add Constant", "add_ci": "Add Constant (In-place)",
    "sum": "Sum", "dot": "Dot Product",
    "max": "Max", "min": "Min",
    "maxIdx": "Max Index", "minIdx": "Min Index",
    "nL1": "Norm L1", "nLinf": "Norm L\u221e",
    "nrm": "Normalize", "nrm_i": "Normalize (In-place)",
    "sos": "Sum of Squares",
}


def _full_op_name(short: str) -> str:
    return OP_FULL_NAMES.get(short, short)


def _status_icon(status: str) -> str:
    return {"PASS": "\u2705", "FAIL": "\u274c", "INTERCEPTED": "\u26a1"}.get(status, "\u2753")


def _parse_config_columns(config_name: str) -> dict:
    """Split 'AVX2 | Cached | Auto | None | Interleaved' into separate columns."""
    parts = [p.strip() for p in config_name.split("|")]
    if len(parts) == 5:
        return {"SIMD": parts[0], "Store": parts[1], "Alignment": parts[2], "Prefetch": parts[3], "ILP": parts[4]}
    return {"SIMD": "\u2014", "Store": "\u2014", "Alignment": "\u2014", "Prefetch": "\u2014", "ILP": "\u2014", "Config": config_name}


def _speedup_color(val):
    """Return CSS for heatmap: green for speedup, red for slowdown, intensity scales with magnitude."""
    if val is None or pd.isna(val):
        return ""
    v = float(val)
    if v >= 1.0:
        intensity = min((v - 1.0) / 4.0, 1.0)
        g = int(60 + intensity * 195)
        return f"color: rgb(0, {g}, 0)"
    else:
        intensity = min((1.0 - v) / 0.8, 1.0)
        r = int(60 + intensity * 195)
        return f"color: rgb({r}, 0, 0)"


def _speedup_bg(val):
    """Return background heatmap for speedup column."""
    if val is None or pd.isna(val):
        return ""
    v = float(val)
    if v >= 1.0:
        intensity = min((v - 1.0) / 4.0, 1.0)
        g = int(20 + intensity * 40)
        return f"background-color: rgba(0, {80 + int(intensity*175)}, 0, 0.15)"
    else:
        intensity = min((1.0 - v) / 0.8, 1.0)
        r = int(20 + intensity * 40)
        return f"background-color: rgba({80 + int(intensity*175)}, 0, 0, 0.15)"


def _build_df(suites: list) -> pd.DataFrame:
    rows = []
    for s in suites:
        config_cols = _parse_config_columns(s.config_name)
        for r in s.results:
            speedup = r.speedup
            row = {
                "Suite": s.suite_name,
                **config_cols,
                "Operation": _full_op_name(r.op),
                "DType": s.dtype,
                "Boundary": s.boundary,
                "Status": r.status,
                "Stalker (\u03bcs)": round(r.time_us, 1),
                "NumPy (\u03bcs)": round(r.py_time_us, 1) if r.py_time_us else None,
                "Speedup": round(speedup, 2) if speedup else None,
                "Rel Err": f"{r.err:.2e}" if r.err else "0",
            }
            rows.append(row)
    return pd.DataFrame(rows)


def render_result_table(suites: list, key_prefix: str = ""):
    """Render separate expandable tables per suite with heatmap speedup."""
    if not suites:
        st.info("No results to display.")
        return

    suite_names = sorted(set(s.suite_name for s in suites))

    for sn in suite_names:
        relevant = [s for s in suites if s.suite_name == sn]
        df = _build_df(relevant)

        # Remove Suite column (redundant within expander)
        if "Suite" in df.columns:
            df = df.drop(columns=["Suite"])
        if "Config" in df.columns:
            df = df.drop(columns=["Config"])

        # Default sort by Operation
        df = df.sort_values("Operation").reset_index(drop=True)

        # Reorder: SIMD config cols, Operation, then rest
        simd_cols = [c for c in ["SIMD", "Store", "Alignment", "Prefetch", "ILP"] if c in df.columns]
        other_cols = [c for c in df.columns if c not in simd_cols and c != "Operation"]
        df = df[simd_cols + ["Operation"] + other_cols]

        pass_count = len(df[df["Status"] == "PASS"])
        total = len(df)
        label = f"{sn}  \u2014  {pass_count}/{total} PASS"

        with st.expander(label, expanded=True):
            styled = df.style
            # Status coloring
            styled = styled.map(
                lambda v: "background-color: #1a3a1a" if v == "PASS"
                else ("background-color: #3a1a1a" if v == "FAIL" else ""),
                subset=["Status"],
            )
            # Speedup heatmap
            if "Speedup" in df.columns:
                styled = styled.map(_speedup_color, subset=["Speedup"])
                styled = styled.map(_speedup_bg, subset=["Speedup"])

            st.dataframe(
                styled,
                use_container_width=True,
                height=min(len(df) * 35 + 40, 800),
                key=f"result_table_{key_prefix}_{sn}",
            )
    return


def render_suite_card(suite: SuiteResults, idx: int = 0, expanded: bool = False, card_key: str = ""):
    """Render a single suite result as an expandable card.

    card_key should change (e.g. include a version counter) when you need to
    force Streamlit to re-create the expander with the new *expanded* value.
    """
    status_str = "\u2705 ALL PASS" if suite.all_passed else f"\u274c {suite.fail_count} FAIL"
    label = f"{suite.config_name}  |  {suite.dtype}  |  {suite.boundary}  \u2014  {status_str}"

    with st.expander(label, expanded=expanded, key=f"sc_{card_key}_{idx}" if card_key else None):
        n = len(suite.results)
        cols_per_row = min(n, 4)
        for row_start in range(0, n, cols_per_row):
            row_end = min(row_start + cols_per_row, n)
            cols = st.columns(row_end - row_start)
            for ci, r in enumerate(suite.results[row_start:row_end]):
                with cols[ci]:
                    icon = _status_icon(r.status)
                    full_name = _full_op_name(r.op)
                    # Single markdown call per cell for performance
                    parts = [f"<span style='font-size:1.05em;font-weight:600'>{icon} {full_name}</span>"]
                    parts.append(f"<br><span style='font-size:0.9em;color:#ccc'>{r.time_us:.0f} \u03bcs</span>")
                    if r.py_time_us:
                        speedup = r.speedup
                        if speedup and speedup >= 1:
                            color, arrow = "#00d4aa", "\u25b2"
                            txt = f"{speedup:.1f}\u00d7 speedup"
                        elif speedup:
                            color, arrow = "#ff6b6b", "\u25bc"
                            txt = f"{speedup:.1f}\u00d7 slower"
                        else:
                            color, arrow, txt = "#888", "", ""
                        parts.append(f"<br><span style='font-size:0.82em;color:{color}'>{arrow} {txt}</span>")
                        parts.append(f"<br><span style='font-size:0.78em;color:#888'>py: {r.py_time_us:.0f} \u03bcs</span>")
                    st.markdown("".join(parts), unsafe_allow_html=True)
