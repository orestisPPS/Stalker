//
// Created by hal9000 on 6/17/24.
//

#ifndef STALKER_SHAREDCACHE_H
#define STALKER_SHAREDCACHE_H
#include "CacheLevel.h"
#include <stdexcept>

class SharedCache {
public:
    SharedCache(CacheLevel *cacheLeve1Data, CacheLevel *cacheLevel1Instructions, CacheLevel *cacheLevel2, CacheLevel *cacheLevel3);
    ~SharedCache() = default;
    const CacheLevel *getCacheLevel1Data() const;
    const CacheLevel *getCacheLevel1Instructions() const;
    const CacheLevel *getCacheLevel2() const;
    const CacheLevel *getCacheLevel3() const;
    const CacheLevel *getCacheLevel(unsigned level) const;
private:
    CacheLevel *_cacheLevel1_Data;
    CacheLevel *_cacheLevel1_Instructions;
    CacheLevel *_cacheLevel2;
    CacheLevel *_cacheLevel3;
};


#endif //STALKER_SHAREDCACHE_H
