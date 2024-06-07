//
// Created by hal9000 on 6/3/24.
//

#include "STLKR_Config_SIMD.h"
constexpr _mm_hint STLKR_Config_SIMD::getHint() const {
    return static_cast<_mm_hint>(_hint);
}

constexpr unsigned STLKR_Config_SIMD::getDistance() const {
    return static_cast<unsigned>(_distance);
}

constexpr STLKR_SIMD_Stores STLKR_Config_SIMD::getStore() const {
    return _store;
}

void STLKR_Config_SIMD::setHint(STLKR_SIMD_PrefetchHint hint) {
    _hint = hint;
}

void STLKR_Config_SIMD::setDistance(unsigned cacheLinesAhead) {
    _distance = cacheLinesAhead;
}

void STLKR_Config_SIMD::setStore(STLKR_SIMD_Stores store) {
    _store = store;
}