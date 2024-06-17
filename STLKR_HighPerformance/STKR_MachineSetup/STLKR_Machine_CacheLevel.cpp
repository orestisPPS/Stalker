//
// Created by hal9000 on 6/15/24.
//

#include "STLKR_Machine_CacheLevel.h"

STLKR_Machine_CacheLevel::STLKR_Machine_CacheLevel(unsigned level, unsigned size, const std::vector<unsigned> &threads) {
    _level = level;
    _size = size;
    _occupiedSpace = 0;
    _threads = threads;
}

unsigned STLKR_Machine_CacheLevel::getLevel() const{
    return _level;
}

unsigned STLKR_Machine_CacheLevel::getSize() const{
    return _size;
}

unsigned STLKR_Machine_CacheLevel::getOccupiedSpace() const{
    return _occupiedSpace;
}

unsigned STLKR_Machine_CacheLevel::getOccupiedSpacePerCent() const{
    return _occupiedSpace * 100 / _size;
}

unsigned STLKR_Machine_CacheLevel::getFreeSpace() const{
    return _size - _occupiedSpace;
}

unsigned STLKR_Machine_CacheLevel::getFreeSpacePerCent() const{
    return ( _size - _occupiedSpace ) * 100 / _size;
}

const std::vector<unsigned> &STLKR_Machine_CacheLevel::getThreads() const{
    return _threads;
}

