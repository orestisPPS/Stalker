"""Overview / landing page."""
import streamlit as st
import pandas as pd
from ..models.results import SessionResults


def render_overview_page(session: SessionResults = None):
    st.title("Staλκer HPC Dashboard")
    st.caption("High-Performance Computing Library — Python Bindings Control Center")

    if session is None or not session.suites:
        st.info("No test session loaded. Run tests from the **Test Runner** page or the **Build & Run** panel.")
        _render_system_info()
        return

    # KPI row
    total = len(session.suites)
    passed = session.total_pass
    failed = session.total_fails
    intercepted = session.total_intercepted

    c1, c2, c3, c4 = st.columns(4)
    c1.metric("Total Configurations", total)
    c2.metric("Passed", passed)
    c3.metric("Failed", failed, delta=f"-{failed}" if failed else None, delta_color="inverse")
    c4.metric("HW Intercepted", intercepted)

    # Summary by suite
    suite_names = sorted(set(s.suite_name for s in session.suites))
    rows = []
    for sn in suite_names:
        relevant = [s for s in session.suites if s.suite_name == sn]
        rows.append({
            "Suite": sn,
            "Configs Tested": len(relevant),
            "Pass": sum(1 for s in relevant if s.all_passed),
            "Fail": sum(s.fail_count for s in relevant),
        })
    st.dataframe(pd.DataFrame(rows), use_container_width=True)

    # Summary by dtype
    st.subheader("Coverage by Data Type")
    dtypes = sorted(set(s.dtype for s in session.suites))
    dtype_rows = []
    for dt in dtypes:
        relevant = [s for s in session.suites if s.dtype == dt]
        dtype_rows.append({
            "DType": dt,
            "Tested": len(relevant),
            "Pass": sum(1 for s in relevant if s.all_passed),
            "Fail": sum(s.fail_count for s in relevant),
        })
    st.dataframe(pd.DataFrame(dtype_rows), use_container_width=True)

    _render_system_info()


def _render_system_info():
    st.divider()
    st.subheader("System Info")
    info = _get_system_info()
    if isinstance(info, dict) and info:
        cols = st.columns(len(info))
        for i, (k, v) in enumerate(info.items()):
            cols[i].metric(k.replace("_", " ").title(), v)
    elif isinstance(info, str):
        st.warning(info)
    else:
        st.caption("Stalker bindings loaded — no extra info attributes exposed.")


@st.cache_data(ttl=3600, show_spinner=False)
def _get_system_info():
    try:
        import stalker
        core = stalker.core
        info_items = {}
        for attr in ("simd_instruction_set", "alignment", "unroll_factor", "num_threads"):
            if hasattr(core, attr):
                info_items[attr] = str(getattr(core, attr))
        return info_items if info_items else None
    except Exception as e:
        return f"Could not load stalker bindings: {e}"
