//
// Created by hal9000 on 6/17/24.
//

#include "SharedCache.h"

SharedCache::SharedCache(CacheLevel *cacheLeve1, CacheLevel *cacheLevel2, CacheLevel *cacheLevel3) {
    _cacheLevel1 = cacheLeve1;
    _cacheLevel2 = cacheLevel2;
    _cacheLevel3 = cacheLevel3;
}

const CacheLevel *SharedCache::getCacheLevel1() const {
    return _cacheLevel1;
}

const CacheLevel *SharedCache::getCacheLevel2() const {
    return _cacheLevel2;
}

const CacheLevel *SharedCache::getCacheLevel3() const {
    return _cacheLevel3;
}

const CacheLevel *SharedCache::getCacheLevel(unsigned level) const {
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
