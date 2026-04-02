"""Build action panel — buttons for C++ build, pip install, test runs."""
import streamlit as st
from ..engine.build import BuildEngine, BuildResult


def _show_result(label: str, result: BuildResult):
    if result.success:
        st.success(f"{label}: completed successfully (exit {result.return_code})")
    else:
        st.error(f"{label}: failed (exit {result.return_code})")
    with st.expander(f"{label} — full log", expanded=not result.success):
        st.code(result.log[-8000:] if len(result.log) > 8000 else result.log, language="text")


def render_build_panel(build_engine: BuildEngine):
    """Render the build control panel with action buttons."""
    st.subheader("Build & Run")

    col1, col2, col3, col4, col5, col6 = st.columns(6)

    with col1:
        if st.button("🔨 C++ Clean Build", use_container_width=True):
            with st.spinner("Running C++ clean build..."):
                _show_result("C++ Clean Build", build_engine.cpp_clean_build())

    with col2:
        if st.button("🔧 C++ Build", use_container_width=True):
            with st.spinner("Running C++ build..."):
                _show_result("C++ Build", build_engine.cpp_build())

    with col3:
        if st.button("📦 Pip Install", use_container_width=True):
            with st.spinner("Installing Python bindings..."):
                _show_result("Pip Install", build_engine.pip_install_bindings())

    with col4:
        if st.button("🧹 Clean Install", use_container_width=True):
            with st.spinner("Clean installing Python bindings..."):
                _show_result("Clean Pip Install", build_engine.pip_clean_install_bindings())

    with col5:
        if st.button("🧪 C++ Tests", use_container_width=True):
            with st.spinner("Running C++ tests..."):
                _show_result("C++ Tests", build_engine.run_cpp_tests())

    with col6:
        if st.button("📊 C++ Benchmarks", use_container_width=True):
            with st.spinner("Running C++ benchmarks..."):
                _show_result("C++ Benchmarks", build_engine.run_cpp_benchmarks())
