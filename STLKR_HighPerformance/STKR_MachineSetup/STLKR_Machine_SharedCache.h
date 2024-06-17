//
// Created by hal9000 on 6/17/24.
//

#ifndef STALKER_STLKR_MACHINE_SHAREDCACHE_H
#define STALKER_STLKR_MACHINE_SHAREDCACHE_H
#include "STLKR_Machine_CacheLevel.h"
#include <stdexcept>

class STLKR_Machine_SharedCache {
public:
    STLKR_Machine_SharedCache(STLKR_Machine_CacheLevel cacheLeve1, STLKR_Machine_CacheLevel cacheLevel2, STLKR_Machine_CacheLevel cacheLevel3);
    STLKR_Machine_CacheLevel &getCacheLevel1();
    STLKR_Machine_CacheLevel &getCacheLevel2();
    STLKR_Machine_CacheLevel &getCacheLevel3();
    STLKR_Machine_CacheLevel &getCacheLevel(unsigned level);
private:
    STLKR_Machine_CacheLevel _cacheLevel1;
    STLKR_Machine_CacheLevel _cacheLevel2;
    STLKR_Machine_CacheLevel _cacheLevel3;
};


#endif //STALKER_STLKR_MACHINE_SHAREDCACHE_H
