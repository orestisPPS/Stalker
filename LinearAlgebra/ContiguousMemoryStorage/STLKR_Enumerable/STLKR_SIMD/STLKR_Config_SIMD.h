//
// Created by hal9000 on 6/3/24.
//

#ifndef STALKER_STLKR_CONFIG_SIMD_H
#define STALKER_STLKR_CONFIG_SIMD_H

#include <immintrin.h>

enum class STLKR_SIMD_PrefetchHint {
    T0 = _MM_HINT_T0,
    T1 = _MM_HINT_T1,
    T2 = _MM_HINT_T2,
    NTA = _MM_HINT_NTA
};


enum class STLKR_SIMD_Stores{
    Temporal = 0,
    NonTemporal = 1
};

struct STLKR_Config_SIMD {
public:

    constexpr inline _mm_hint getHint() const;
    constexpr inline unsigned getDistance() const;
    constexpr inline STLKR_SIMD_Stores getStore() const;
    void setHint(STLKR_SIMD_PrefetchHint hint);
    void setDistance(unsigned cacheLinesAhead);
    void setStore(STLKR_SIMD_Stores store);
private:
    STLKR_SIMD_PrefetchHint _hint = STLKR_SIMD_PrefetchHint::T0;
    unsigned _distance = 1;
    STLKR_SIMD_Stores _store = STLKR_SIMD_Stores::Temporal;
};


#endif //STALKER_STLKR_CONFIG_SIMD_H
