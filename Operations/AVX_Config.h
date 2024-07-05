//
// Created by hal9000 on 6/3/24.
//

#ifndef STALKER_AVX_CONFIG_H
#define STALKER_AVX_CONFIG_H

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

struct AVX_Config {
public:

    constexpr inline _mm_hint getHint() const {
        return static_cast<_mm_hint>(_hint);
    }
    
    constexpr inline unsigned getDistance() const{
        return _distance;
    }
    
    constexpr inline STLKR_SIMD_Stores getStore() const{
        return _store;
    }
    
    void setHint(STLKR_SIMD_PrefetchHint hint){
        _hint = hint;
    }
    void setDistance(unsigned cacheLinesAhead){
        _distance = cacheLinesAhead;
    }
    void setStore(STLKR_SIMD_Stores store){
        _store = store;
    }
private:
    STLKR_SIMD_PrefetchHint _hint = STLKR_SIMD_PrefetchHint::T0;
    unsigned _distance = 1;
    STLKR_SIMD_Stores _store = STLKR_SIMD_Stores::Temporal;
};


#endif //STALKER_AVX_CONFIG_H
