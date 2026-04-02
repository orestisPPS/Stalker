"""Test runner page — execute binding tests live and display results."""
import streamlit as st
from ..engine.runner import TestEngine
from ..models.results import SessionResults, SuiteResults
from ..components.result_card import render_suite_card, render_result_table
from ..components.charts import render_timing_chart, render_speedup_chart


# ── Helpers ──────────────────────────────────────────────────────────

def _categorize(config_name: str) -> str:
    if config_name.startswith("Scalar"):
        return "Scalar"
    if config_name.startswith("Unrolled"):
        return "Unrolled"
    return "SIMD"


def _parse_simd_parts(config_name: str):
    parts = [p.strip() for p in config_name.split("|")]
    return parts if len(parts) == 5 else None


# ── Three-category filter ────────────────────────────────────────────

def _render_config_filter(session: SessionResults):
    """Filter results with three config categories: Scalar, Unrolled, SIMD."""
    st.subheader("Filter Results")
    st.caption("Leave a field empty to show all values for that dimension.")

    suites = session.suites

    # Gather unique values
    all_dtypes = sorted(set(s.dtype for s in suites))
    all_suite_names = sorted(set(s.suite_name for s in suites))
    all_ops = sorted(set(r.op for s in suites for r in s.results))
    all_boundaries = sorted(set(s.boundary for s in suites))

    scalar_names = sorted(set(s.config_name for s in suites if _categorize(s.config_name) == "Scalar"))
    unrolled_names = sorted(set(s.config_name for s in suites if _categorize(s.config_name) == "Unrolled"))

    simd_dims = {"type": set(), "store": set(), "align": set(), "pref": set(), "ilp": set()}
    for s in suites:
        parts = _parse_simd_parts(s.config_name)
        if parts:
            simd_dims["type"].add(parts[0])
            simd_dims["store"].add(parts[1])
            simd_dims["align"].add(parts[2])
            simd_dims["pref"].add(parts[3])
            simd_dims["ilp"].add(parts[4])

    # ── Common filters ──
    c1, c2, c3, c4 = st.columns(4)
    with c1:
        f_dtype = st.multiselect("Data Type", all_dtypes, key="f_dtype")
    with c2:
        f_suite = st.multiselect("Suite", all_suite_names, key="f_suite")
    with c3:
        f_op = st.multiselect("Operation", all_ops, key="f_op")
    with c4:
        f_boundary = st.multiselect("Boundary", all_boundaries, key="f_boundary")

    st.divider()

    # ── Scalar category ──
    f_scalar = []
    with st.expander("⚙️ Scalar", expanded=False):
        enable_scalar = st.toggle("Enable Scalar configs", value=True, key="f_enable_scalar")
        if enable_scalar:
            f_scalar = st.multiselect("Variant (Loop / STD)", scalar_names, key="f_scalar_var")

    # ── Unrolled category ──
    f_unrolled = []
    with st.expander("🔄 Unrolled", expanded=False):
        enable_unrolled = st.toggle("Enable Unrolled configs", value=True, key="f_enable_unrolled")
        if enable_unrolled:
            f_unrolled = st.multiselect("Variant", unrolled_names, key="f_unrolled_var")

    # ── SIMD category ──
    f_simd_type, f_store, f_align, f_pref, f_ilp = [], [], [], [], []
    with st.expander("⚡ SIMD", expanded=False):
        enable_simd = st.toggle("Enable SIMD configs", value=True, key="f_enable_simd")
        if enable_simd:
            s1, s2, s3 = st.columns(3)
            with s1:
                f_simd_type = st.multiselect("SIMD Type", sorted(simd_dims["type"]), key="f_simd_type")
            with s2:
                f_store = st.multiselect("Store", sorted(simd_dims["store"]), key="f_store")
            with s3:
                f_align = st.multiselect("Alignment", sorted(simd_dims["align"]), key="f_align")
            s4, s5 = st.columns(2)
            with s4:
                f_pref = st.multiselect("Prefetch", sorted(simd_dims["pref"]), key="f_pref")
            with s5:
                f_ilp = st.multiselect("ILP", sorted(simd_dims["ilp"]), key="f_ilp")

    # ── Apply filters ──
    filtered = []
    for s in suites:
        cat = _categorize(s.config_name)

        # Per-category filter
        if cat == "Scalar":
            if not enable_scalar:
                continue
            if f_scalar and s.config_name not in f_scalar:
                continue
        elif cat == "Unrolled":
            if not enable_unrolled:
                continue
            if f_unrolled and s.config_name not in f_unrolled:
                continue
        else:
            if not enable_simd:
                continue
            parts = _parse_simd_parts(s.config_name)
            if parts:
                if f_simd_type and parts[0] not in f_simd_type:
                    continue
                if f_store and parts[1] not in f_store:
                    continue
                if f_align and parts[2] not in f_align:
                    continue
                if f_pref and parts[3] not in f_pref:
                    continue
                if f_ilp and parts[4] not in f_ilp:
                    continue

        # Common filters
        if f_dtype and s.dtype not in f_dtype:
            continue
        if f_suite and s.suite_name not in f_suite:
            continue
        if f_boundary and s.boundary not in f_boundary:
            continue
        if f_op:
            op_filtered = [r for r in s.results if r.op in f_op]
            if not op_filtered:
                continue
            s = SuiteResults(
                suite_name=s.suite_name, config_name=s.config_name,
                dtype=s.dtype, boundary=s.boundary, results=op_filtered,
            )
        filtered.append(s)

    return filtered


def render_test_runner_page():
    st.header("Test Runner")

    # ── Test Parameters ──────────────────────────────────────────────
    with st.expander("Test Parameters", expanded=True):
        col1, col2 = st.columns(2)
        with col1:
            size = st.number_input("Vector Size", min_value=100, max_value=100_000_000,
                                   value=1_000_000, step=100_000)
            suites = st.multiselect("Suites", ["memory", "math"], default=["memory", "math"])
        with col2:
            all_dtypes = ["float32", "float64", "int32", "uint32", "int16"]
            dtypes = st.multiselect("Data Types", all_dtypes, default=all_dtypes)
            boundaries = st.multiselect("Memory Boundaries",
                                        ["Auto", "Aligned (64B)", "Unaligned"],
                                        default=["Auto"])

        # Resolve "Auto" boundary to both aligned and unaligned
        resolved_boundaries = boundaries[:]
        if "Auto" in resolved_boundaries:
            resolved_boundaries.remove("Auto")
            if "Aligned (64B)" not in resolved_boundaries:
                resolved_boundaries.append("Aligned (64B)")
            if "Unaligned" not in resolved_boundaries:
                resolved_boundaries.append("Unaligned")

    # ── Run ──────────────────────────────────────────────────────────
    col_run, col_icon = st.columns([6, 1])
    with col_run:
        run_clicked = st.button("▶  Run Tests", type="primary", use_container_width=True)

    if run_clicked:
        engine = TestEngine()
        progress_bar = st.progress(0, text="Initializing...")

        def on_progress(cur, total, desc):
            progress_bar.progress(cur / total, text=desc)

        session = engine.run(
            suites=suites, size=size, dtypes=dtypes,
            boundaries=resolved_boundaries, progress_callback=on_progress,
        )
        progress_bar.progress(1.0, text="Complete!")

        st.session_state["last_session"] = session
        st.session_state["test_ran"] = True
        st.rerun()

    # ── Display results ──────────────────────────────────────────────
    session: SessionResults = st.session_state.get("last_session")
    if session and session.suites:
        st.success(f"Session: {len(session.suites)} configs  |  "
                   f"✅ {session.total_pass}  ❌ {session.total_fails}  ⚡ {session.total_intercepted}")

        # Config filter (merged from config search)
        with st.expander("🔍 Filter Results", expanded=False):
            display_suites = _render_config_filter(session)
        # Fallback: if no filter applied, show all
        if display_suites is None:
            display_suites = session.suites

        if not display_suites:
            st.warning("No results match the current filters.")
            return

        tab_cards, tab_table, tab_charts = st.tabs(["Cards", "Table", "Charts"])

        # ── Cards tab with key-versioned expand / collapse ────────────
        with tab_cards:
            suite_names = sorted(set(s.suite_name for s in display_suites))
            for sn in suite_names:
                relevant = [s for s in display_suites if s.suite_name == sn]
                st.markdown(f"### {sn}")

                # Version counter — bump to force Streamlit to recreate expanders
                ver_key = f"card_ver_{sn}"
                exp_key = f"card_exp_{sn}"
                if ver_key not in st.session_state:
                    st.session_state[ver_key] = 0
                if exp_key not in st.session_state:
                    st.session_state[exp_key] = False

                col_a, col_b, _ = st.columns([1, 1, 6])
                with col_a:
                    if st.button("Expand All", key=f"expand_{sn}"):
                        st.session_state[exp_key] = True
                        st.session_state[ver_key] += 1
                        st.rerun()
                with col_b:
                    if st.button("Collapse All", key=f"collapse_{sn}"):
                        st.session_state[exp_key] = False
                        st.session_state[ver_key] += 1
                        st.rerun()

                ver = st.session_state[ver_key]
                expanded = st.session_state[exp_key]
                for i, s in enumerate(relevant):
                    render_suite_card(s, idx=i, expanded=expanded, card_key=f"{sn}_v{ver}")

        # ── Table tab ────────────────────────────────────────────────
        with tab_table:
            render_result_table(display_suites, key_prefix="runner")

        # ── Charts tab ───────────────────────────────────────────────
        with tab_charts:
            all_configs = sorted(set(s.config_name for s in display_suites))
            selected_config = st.selectbox(
                "Configuration", ["All (averaged)"] + all_configs,
                key="chart_config_select",
            )
            if selected_config == "All (averaged)":
                chart_suites = display_suites
            else:
                chart_suites = [s for s in display_suites if s.config_name == selected_config]

            if chart_suites:
                render_timing_chart(chart_suites, title="Stalker vs NumPy — Kernel Timing")
                render_speedup_chart(chart_suites, title="Speedup over NumPy")
            else:
                st.info("No data for the selected configuration.")
