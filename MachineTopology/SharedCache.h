//
// Created by hal9000 on 6/17/24.
//

#ifndef STALKER_SHAREDCACHE_H
#define STALKER_SHAREDCACHE_H
#include "CacheLevel.h"
#include <stdexcept>
#include <unordered_map>

class SharedCache {
public:
    SharedCache(CacheLevel *l1Data, CacheLevel *l1Instructions, CacheLevel *l2, CacheLevel *l3) {
        _cacheLevels = { {L1_Data, l1Data}, {L1_Instructions, l1Instructions}, {L2, l2}, {L3, l3} };
    }
    
    ~SharedCache() = default;

    inline const CacheLevel* getCacheLevel(CacheLevelType level) const {
        auto it = _cacheLevels.find(level);
        if (it != _cacheLevels.end()) {
            return it->second;
        }
        return nullptr;
    }
private:
    std::unordered_map<CacheLevelType, CacheLevel*> _cacheLevels;
};


#endif //STALKER_SHAREDCACHE_H
