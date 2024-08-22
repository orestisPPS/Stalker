//
// Created by hal9000 on 6/15/24.
//

#ifndef STALKER_CACHELEVEL_H
#define STALKER_CACHELEVEL_H

#include <iostream>
#include <vector>

enum CacheLevelType {
    L1_Data = 0,
    L1_Instructions = 1,
    L2 = 2,
    L3 = 3,
    LNone = -1
};

class CacheLevel {
public:
    CacheLevel(CacheLevelType level, unsigned size , const std::vector<unsigned> &threads) :
    _level(level), _size(size), _threads(threads) {}
    CacheLevel() = default;
    ~CacheLevel() = default;
    
    [[nodiscard]] inline unsigned getLevel() const { return _level; }
    [[nodiscard]] inline unsigned getSize() const { return _size; }
    [[nodiscard]] inline constexpr unsigned getSizeInCacheLines() const { return (_size + 63) / 64; }
    [[nodiscard]] inline const std::vector<unsigned> &getThreads() const{ return _threads; }
private:
    CacheLevelType _level;
    unsigned _size;
    std::vector<unsigned> _threads;
};


#endif //STALKER_CACHELEVEL_H
