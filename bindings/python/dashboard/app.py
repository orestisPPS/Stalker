"""
Staλκer HPC Dashboard — Streamlit Application
Launch:  streamlit run bindings/python/dashboard/app.py
"""
import os
import sys
import streamlit as st

# Ensure project root and bindings dir are on the path
_PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
_BINDINGS_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
for _p in (_PROJECT_ROOT, _BINDINGS_DIR):
    if _p not in sys.path:
        sys.path.insert(0, _p)

from dashboard.engine.build import BuildEngine
from dashboard.pages.overview import render_overview_page
from dashboard.pages.test_runner import render_test_runner_page
from dashboard.pages.benchmarks import render_benchmarks_page
from dashboard.pages.build_ops import render_build_ops_page

# ── Page config ──────────────────────────────────────────────────────
st.set_page_config(
    page_title="Staλκer HPC",
    page_icon="⚡",
    layout="wide",
    initial_sidebar_state="collapsed",
)

# ── Custom CSS ───────────────────────────────────────────────────────
_CSS = """
<style>
    .stApp { background-color: #0e1117; }
    .stMetric { background-color: #161b22; border-radius: 8px; padding: 12px; }
    div[data-testid="stExpander"] { border: 1px solid #30363d; border-radius: 8px; }
    .stTabs [data-baseweb="tab"] { font-weight: 600; }
    section[data-testid="stSidebar"] { display: none !important; }
    .zone-banner { text-align: center; padding: 8px 0 4px 0; }
    .zone-banner pre {
        color: #00d4aa; font-size: 0.55em; line-height: 1.15;
        font-family: 'Courier New', monospace; margin: 0 auto;
        display: inline-block; text-align: left;
    }
    .zone-subtitle {
        color: #8b949e; font-size: 0.85em; letter-spacing: 0.15em;
        text-transform: uppercase; margin-top: 2px;
    }
</style>
"""
if "css_injected" not in st.session_state:
    st.markdown(_CSS, unsafe_allow_html=True)
    st.session_state["css_injected"] = True
else:
    st.markdown(_CSS, unsafe_allow_html=True)

# ── ASCII banner ─────────────────────────────────────────────────────
BANNER = r"""
 _____ _            _____
|_   _| |__   ___  |__  /___  _ __   ___
  | | | '_ \ / _ \   / // _ \| '_ \ / _ \
  | | | | | |  __/  / /| (_) | | | |  __/
  |_| |_| |_|\___| /____\___/|_| |_|\___|
"""

st.markdown(
    '<div class="zone-banner">'
    f'<pre>{BANNER}</pre>'
    '<div class="zone-subtitle">Testing &amp; Benchmarking environment for Sta&lambda;ker</div>'
    '</div>',
    unsafe_allow_html=True,
)

# ── Shared state ─────────────────────────────────────────────────────
BENCHMARK_LOGS_DIR = os.path.join(_PROJECT_ROOT, "Benchmarks", "logs")
build_engine = BuildEngine(_PROJECT_ROOT)

# ── Top-level tabs ───────────────────────────────────────────────────
tab_overview, tab_tests, tab_bench, tab_build = st.tabs(
    ["Overview", "Tests", "Benchmarks", "Build & Ops"]
)

with tab_overview:
    render_overview_page(st.session_state.get("last_session"))

with tab_tests:
    render_test_runner_page()

with tab_bench:
    render_benchmarks_page(BENCHMARK_LOGS_DIR)

with tab_build:
    render_build_ops_page(build_engine)
