"""SIMD configuration picker — lets users build custom config queries."""
import streamlit as st
from ..models.config import SIMDType, StorePolicy, AlignmentPolicy, PrefetchHint, ILPPolicy, SIMDConfig


def render_config_picker(key_prefix: str = "picker") -> SIMDConfig:
    """Render dropdown selectors for each SIMD config axis. Returns a SIMDConfig."""
    cols = st.columns(5)
    with cols[0]:
        simd = st.selectbox("SIMD Type", [e.value for e in SIMDType], key=f"{key_prefix}_simd")
    with cols[1]:
        store = st.selectbox("Store Policy", [e.value for e in StorePolicy], key=f"{key_prefix}_store")
    with cols[2]:
        align = st.selectbox("Alignment", [e.value for e in AlignmentPolicy], key=f"{key_prefix}_align")
    with cols[3]:
        pref = st.selectbox("Prefetch", [e.value for e in PrefetchHint], key=f"{key_prefix}_pref")
    with cols[4]:
        ilp = st.selectbox("ILP Policy", [e.value for e in ILPPolicy], key=f"{key_prefix}_ilp")

    return SIMDConfig(
        simd_type=SIMDType(simd),
        store=StorePolicy(store),
        alignment=AlignmentPolicy(align),
        prefetch=PrefetchHint(pref),
        ilp=ILPPolicy(ilp),
    )
