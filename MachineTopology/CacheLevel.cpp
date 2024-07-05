//
// Created by hal9000 on 6/15/24.
//

#include "CacheLevel.h"

CacheLevel::CacheLevel(unsigned level, unsigned size, const std::vector<unsigned> &threads) {
    _level = level;
    _size = size;
    _occupiedSpace = 0;
    _threads = threads;
}

unsigned CacheLevel::getLevel() const{
    return _level;
}

unsigned CacheLevel::getSize() const{
    return _size;
}

unsigned CacheLevel::getOccupiedSpace() const{
    return _occupiedSpace;
}

unsigned CacheLevel::getOccupiedSpacePerCent() const{
    return _occupiedSpace * 100 / _size;
}

unsigned CacheLevel::getFreeSpace() const{
    return _size - _occupiedSpace;
}

unsigned CacheLevel::getFreeSpacePerCent() const{
    return ( _size - _occupiedSpace ) * 100 / _size;
}

const std::vector<unsigned> &CacheLevel::getThreads() const{
    return _threads;
}

