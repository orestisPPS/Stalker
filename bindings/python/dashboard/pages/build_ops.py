"""Build & Operations page — C++ / Python build controls with live terminal output."""
import streamlit as st
import json
import os
import time
from ..engine.build import BuildEngine

# ── Terminal CSS (injected once) ─────────────────────────────────────
_TERM_CSS = """
<style>
.live-term {
    background: #0d1117; color: #c9d1d9; font-family: 'Courier New', monospace;
    font-size: 0.82em; line-height: 1.4; padding: 12px; border-radius: 6px;
    border: 1px solid #30363d; max-height: 500px; overflow-y: auto;
    white-space: pre-wrap; word-break: break-all;
}
.live-term .cmd { color: #00d4aa; font-weight: bold; }
</style>
"""


def _term_html(cmd: str, output: str, running: bool = False) -> str:
    """Render terminal-style HTML."""
    import html as _html
    safe = _html.escape(output)
    cursor = "█" if running else ""
    return (
        f'{_TERM_CSS}'
        f'<div class="live-term" id="term-scroll">'
        f'<span class="cmd">$ {_html.escape(cmd)}</span>\n{safe}{cursor}'
        f'</div>'
        f'<script>var e=document.getElementById("term-scroll");if(e)e.scrollTop=e.scrollHeight;</script>'
    )


# ── Live terminal helper ─────────────────────────────────────────────

def _run_live(label: str, run_fn, key: str):
    """Execute a build command with real-time streaming terminal output."""
    state_key = f"term_{key}"
    lines: list[str] = []
    term = st.empty()
    status = st.empty()

    status.info(f"⏳ {label}...")
    term.html(_term_html(label, "", running=True))

    last_update = [0.0]

    def on_line(line):
        lines.append(line)
        now = time.monotonic()
        # Throttle UI updates to ~10 fps to keep Streamlit responsive
        if now - last_update[0] > 0.1:
            last_update[0] = now
            term.html(_term_html(label, "".join(lines), running=True))

    result = run_fn(on_line=on_line)

    final = "".join(lines)
    term.html(_term_html(label, final, running=False))

    if result.success:
        status.success(f"✅ {label} — exit {result.return_code}")
    else:
        status.error(f"❌ {label} — exit {result.return_code}")

    st.session_state[state_key] = {"log": final, "ok": result.success, "label": label}


# ── Build config editor ─────────────────────────────────────────────

_PROFILES = ["debug", "release", "perf", "relwithdebinfo", "custom"]
_SIMD_ISA = ["avx2", "avx512", "auto", "none"]
_STORE_POLICIES = ["cache", "stream"]
_ILP_POLICIES = ["interleaved", "grouped"]
_PREFETCH_HINTS = ["HintNone", "HintT0", "HintT1", "HintT2", "HintNTA"]


def _render_config_editor(build_engine: BuildEngine):
    """Editable build configuration derived from stalker-build.json."""
    cfg = build_engine.read_build_config()
    if not cfg:
        st.warning("Could not read stalker-build.json")
        return

    st.subheader("Build Configuration")
    st.caption("Edit settings below and press **Save** to write stalker-build.json.")

    changed = False

    # ── Build settings ──
    with st.expander("🔧 Build Settings", expanded=True):
        c1, c2 = st.columns(2)
        with c1:
            new_dir = st.text_input("Build Directory", value=cfg.get("build_directory", "build"), key="cfg_dir")
            new_profile = st.selectbox(
                "Build Profile", _PROFILES,
                index=_PROFILES.index(cfg.get("build_profile", "release")),
                key="cfg_profile",
            )
            new_jobs = st.number_input("Parallel Jobs", min_value=1, max_value=128,
                                       value=cfg.get("build_jobs", 16), key="cfg_jobs")
        with c2:
            new_tests = st.checkbox("Build Tests", value=cfg.get("build_tests", True), key="cfg_tests")
            new_bench = st.checkbox("Build Benchmarks", value=cfg.get("build_benchmarks", True), key="cfg_bench")
            new_examples = st.checkbox("Build Examples", value=cfg.get("build_examples", True), key="cfg_examples")
            if new_profile == "custom":
                raw_flags = st.text_input("Custom Flags (space separated)",
                                          value=" ".join(cfg.get("build_custom_flags", [])),
                                          key="cfg_custom_flags")
            else:
                raw_flags = ""

    # ── Alignment ──
    with st.expander("📐 Alignment", expanded=False):
        new_align = st.number_input("Alignment (bytes, power of 2)", min_value=1, max_value=4096,
                                    value=cfg.get("alignment", 64), step=1, key="cfg_align")

    # ── SIMD ──
    with st.expander("⚡ SIMD", expanded=False):
        new_simd_enable = st.checkbox("Enable SIMD", value=cfg.get("simd_enable", True), key="cfg_simd_en")
        if new_simd_enable:
            c1, c2 = st.columns(2)
            with c1:
                new_simd_isa = st.selectbox(
                    "Instruction Set", _SIMD_ISA,
                    index=_SIMD_ISA.index(cfg.get("simd_default_instructions", "avx2")),
                    key="cfg_simd_isa",
                )
                new_simd_store = st.selectbox(
                    "Store Policy", _STORE_POLICIES,
                    index=_STORE_POLICIES.index(cfg.get("simd_store_type", "cache")),
                    key="cfg_simd_store",
                )
            with c2:
                new_simd_ilp = st.selectbox(
                    "ILP Policy", _ILP_POLICIES,
                    index=_ILP_POLICIES.index(cfg.get("simd_ilp_policy", "interleaved")),
                    key="cfg_simd_ilp",
                )
                cur_pref = cfg.get("simd_prefetch_hint", "HintNone")
                pref_idx = _PREFETCH_HINTS.index(cur_pref) if cur_pref in _PREFETCH_HINTS else 0
                new_simd_pref = st.selectbox(
                    "Prefetch Hint", _PREFETCH_HINTS, index=pref_idx, key="cfg_simd_pref",
                )
        else:
            new_simd_isa = cfg.get("simd_default_instructions", "avx2")
            new_simd_store = cfg.get("simd_store_type", "cache")
            new_simd_ilp = cfg.get("simd_ilp_policy", "interleaved")
            new_simd_pref = cfg.get("simd_prefetch_hint", "HintNone")

    # ── Unroll ──
    with st.expander("🔁 Unroll", expanded=False):
        new_unroll = st.number_input("Unroll Factor", min_value=1, max_value=64,
                                     value=cfg.get("unroll_factor", 1), key="cfg_unroll")

    # ── Threading ──
    with st.expander("🧵 Threading", expanded=False):
        new_thr_enable = st.checkbox("Enable Threading", value=cfg.get("threading_enable", True), key="cfg_thr_en")
        if new_thr_enable:
            c1, c2 = st.columns(2)
            with c1:
                new_thr_max = st.checkbox("Max Threads (all cores)", value=cfg.get("threading_max_threads_enable", False), key="cfg_thr_max")
                new_thr_num = st.number_input("Num Threads (0=auto)", min_value=0, max_value=1024,
                                              value=cfg.get("threading_num_threads", 1), key="cfg_thr_num")
            with c2:
                new_thr_std = st.checkbox("std::thread backend", value=cfg.get("threading_std_enable", True), key="cfg_thr_std")
                new_thr_posix = st.checkbox("pthread backend", value=cfg.get("threading_posix_enable", False), key="cfg_thr_posix")
                new_thr_smt = st.checkbox("POSIX SMT (hyperthreading)", value=cfg.get("threading_posix_smt_enable", False), key="cfg_thr_smt")
            # Validate mutual exclusion
            if new_thr_std and new_thr_posix:
                st.warning("⚠️ Cannot enable both std::thread and pthread simultaneously. Posix will take precedence.")
            if new_thr_smt and not new_thr_posix:
                st.info("ℹ️ SMT only effective with pthread backend.")
        else:
            new_thr_max = cfg.get("threading_max_threads_enable", False)
            new_thr_num = cfg.get("threading_num_threads", 1)
            new_thr_std = cfg.get("threading_std_enable", True)
            new_thr_posix = cfg.get("threading_posix_enable", False)
            new_thr_smt = cfg.get("threading_posix_smt_enable", False)

    # ── Save button ──
    if st.button("💾 Save Configuration", use_container_width=True, type="primary"):
        cfg["build_directory"] = new_dir
        cfg["build_profile"] = new_profile
        cfg["build_jobs"] = new_jobs
        cfg["build_tests"] = new_tests
        cfg["build_benchmarks"] = new_bench
        cfg["build_examples"] = new_examples
        cfg["build_custom_flags"] = raw_flags.split() if raw_flags.strip() else []
        cfg["alignment"] = new_align
        cfg["simd_enable"] = new_simd_enable
        cfg["simd_default_instructions"] = new_simd_isa
        cfg["simd_store_type"] = new_simd_store
        cfg["simd_ilp_policy"] = new_simd_ilp
        cfg["simd_prefetch_hint"] = new_simd_pref
        cfg["unroll_factor"] = new_unroll
        cfg["threading_enable"] = new_thr_enable
        cfg["threading_max_threads_enable"] = new_thr_max
        cfg["threading_num_threads"] = new_thr_num
        cfg["threading_std_enable"] = new_thr_std
        cfg["threading_posix_enable"] = new_thr_posix
        cfg["threading_posix_smt_enable"] = new_thr_smt
        build_engine.write_build_config(cfg)
        st.success("Configuration saved to stalker-build.json")


# ── Main page ────────────────────────────────────────────────────────

def render_build_ops_page(build_engine: BuildEngine):
    st.header("Build & Operations")

    # ── Build Configuration Editor ───────────────────────────────────
    _render_config_editor(build_engine)

    st.divider()

    # ── C++ Build Commands ───────────────────────────────────────────
    st.subheader("C++ Build Commands")

    cpp_col1, cpp_col2, cpp_col3, cpp_col4 = st.columns(4)
    with cpp_col1:
        if st.button("⚙️ Configure (CMake)", use_container_width=True, key="cpp_configure"):
            _run_live("CMake Configure", build_engine.cpp_configure, "cpp_configure")
    with cpp_col2:
        if st.button("🔨 Build", use_container_width=True, key="cpp_build"):
            _run_live("C++ Build", build_engine.cpp_build, "cpp_build")
    with cpp_col3:
        if st.button("🔨 Clean Build", use_container_width=True, key="cpp_clean_build"):
            _run_live("C++ Clean Build", build_engine.cpp_clean_build, "cpp_clean_build")
    with cpp_col4:
        if st.button("🧹 Clean", use_container_width=True, key="cpp_clean"):
            _run_live("C++ Clean", build_engine.cpp_clean, "cpp_clean")

    # Show last C++ terminal output
    for k in ("cpp_configure", "cpp_build", "cpp_clean_build", "cpp_clean"):
        state = st.session_state.get(f"term_{k}")
        if state:
            icon = "✅" if state["ok"] else "❌"
            with st.expander(f"{icon} {state['label']}", expanded=True):
                log = state["log"][-10000:] if len(state["log"]) > 10000 else state["log"]
                st.html(_term_html(state["label"], log, running=False))

    st.divider()

    # ── Python Build Commands ────────────────────────────────────────
    st.subheader("Python Binding Commands")

    py_col1, py_col2, py_col3 = st.columns(3)
    with py_col1:
        if st.button("📦 Install (pip -e .)", use_container_width=True, key="py_install"):
            _run_live("Pip Install", build_engine.pip_install_bindings, "py_install")
    with py_col2:
        if st.button("🧹 Clean Install (uninstall + install)", use_container_width=True, key="py_clean_install"):
            _run_live("Pip Clean Install", build_engine.pip_clean_install_bindings, "py_clean_install")
    with py_col3:
        if st.button("🗑️ Uninstall", use_container_width=True, key="py_uninstall"):
            _run_live("Pip Uninstall", build_engine.pip_uninstall, "py_uninstall")

    # Show last Python terminal output
    for k in ("py_install", "py_clean_install", "py_uninstall"):
        state = st.session_state.get(f"term_{k}")
        if state:
            icon = "✅" if state["ok"] else "❌"
            with st.expander(f"{icon} {state['label']}", expanded=True):
                log = state["log"][-10000:] if len(state["log"]) > 10000 else state["log"]
                st.html(_term_html(state["label"], log, running=False))
