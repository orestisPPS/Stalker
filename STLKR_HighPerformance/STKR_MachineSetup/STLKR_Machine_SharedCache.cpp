//
// Created by hal9000 on 6/17/24.
//

#include "STLKR_Machine_SharedCache.h"

STLKR_Machine_SharedCache::STLKR_Machine_SharedCache(STLKR_Machine_CacheLevel cacheLeve1, STLKR_Machine_CacheLevel cacheLevel2, STLKR_Machine_CacheLevel cacheLevel3) {
    _cacheLevel1 = cacheLeve1;
    _cacheLevel2 = cacheLevel2;
    _cacheLevel3 = cacheLevel3;
}

STLKR_Machine_CacheLevel &STLKR_Machine_SharedCache::getCacheLevel1() {
    return _cacheLevel1;
}

STLKR_Machine_CacheLevel &STLKR_Machine_SharedCache::getCacheLevel2() {
    return _cacheLevel2;
}

STLKR_Machine_CacheLevel &STLKR_Machine_SharedCache::getCacheLevel3() {
    return _cacheLevel3;
}

STLKR_Machine_CacheLevel &STLKR_Machine_SharedCache::getCacheLevel(unsigned level) {
    switch (level) {
        case 1:
            return _cacheLevel1;
        case 2:
            return _cacheLevel2;
        case 3:
            return _cacheLevel3;
        default:
            throw std::invalid_argument("Invalid cache level");
    }
}