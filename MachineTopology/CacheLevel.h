//
// Created by hal9000 on 6/15/24.
//

#ifndef STALKER_CACHELEVEL_H
#define STALKER_CACHELEVEL_H

#include <iostream>
#include <vector>

class CacheLevel {
    
public:
    CacheLevel(unsigned level, unsigned size , const std::vector<unsigned> &threads);
        
    CacheLevel() = default;
    
    ~CacheLevel() = default;
    
    unsigned getLevel() const;
    unsigned getSize() const;
    constexpr unsigned getSizeInCacheLines() const;
    const std::vector<unsigned> &getThreads() const;
private:
    unsigned _level;
    unsigned _size;
    std::vector<unsigned> _threads;

};


#endif //STALKER_CACHELEVEL_H
