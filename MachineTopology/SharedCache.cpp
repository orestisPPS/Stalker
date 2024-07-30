//
// Created by hal9000 on 6/17/24.
//

#include "SharedCache.h"

SharedCache::SharedCache(CacheLevel *cacheLevel1Data, CacheLevel *cacheLevel1Instructions, CacheLevel *cacheLevel2, CacheLevel *cacheLevel3) :
        _cacheLevel1_Data(cacheLevel1Data), _cacheLevel1_Instructions(cacheLevel1Instructions), _cacheLevel2(cacheLevel2), _cacheLevel3(cacheLevel3) {}

const CacheLevel *SharedCache::getCacheLevel1Data() const {
    return _cacheLevel1_Data;
}

const CacheLevel *SharedCache::getCacheLevel1Instructions() const {
    return _cacheLevel1_Instructions;
}

const CacheLevel *SharedCache::getCacheLevel2() const {
    return _cacheLevel2;
}

const CacheLevel *SharedCache::getCacheLevel3() const {
    return _cacheLevel3;
}

