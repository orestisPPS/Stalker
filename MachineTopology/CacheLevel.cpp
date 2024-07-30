//
// Created by hal9000 on 6/15/24.
//

#include "CacheLevel.h"

CacheLevel::CacheLevel(unsigned level, unsigned size, const std::vector<unsigned> &threads, CacheLevelType type) : 
        _level(level), _size(size), _threads(threads), _type(type) {}

unsigned CacheLevel::getLevel() const{
    return _level;
}

unsigned CacheLevel::getSize() const{
    return _size;
}

constexpr unsigned CacheLevel::getSizeInCacheLines() const{
    return (_size + 63) / 64;
}

const std::vector<unsigned> &CacheLevel::getThreads() const {
    return _threads;
}

const CacheLevelType CacheLevel::getType() const {
    return _type;
}
