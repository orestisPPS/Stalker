//
// Created by hal9000 on 6/8/24.
//

#ifndef STALKER_STLKR_SIMD_CACHELINEINSTANCE_H
#define STALKER_STLKR_SIMD_CACHELINEINSTANCE_H

#endif //STALKER_STLKR_SIMD_CACHELINEINSTANCE_H 

template <typename T>
class STLKR_Thread_CacheLineInstance {
public:
    
    STLKR_Thread_CacheLineInstance() = default;
    
    STLKR_Thread_CacheLineInstance(unsigned id, unsigned startIndex, unsigned endIndex,
                                 unsigned L1CacheSizeB, unsigned L2CacheSizeB, unsigned L3CacheSizeB, unsigned cacheLineSize = 64){
        _id = id;
        _startIndex = startIndex;
        _endIndex = endIndex;
        _cacheSizeB[0] = L1CacheSizeB;
        _cacheSizeB[1] = L2CacheSizeB;
        _cacheSizeB[2] = L3CacheSizeB;
        _cacheLines[0] = _calculateCacheLines(_cacheSizeB[0]);
        _cacheLines[1] = _calculateCacheLines(_cacheSizeB[1]);
        _cacheLines[2] = _calculateCacheLines(_cacheSizeB[2]);
        _elementsPerCacheLine = cacheLineSize / sizeof(T);
    }
    
private:
    unsigned _id;
    unsigned _startIndex;
    unsigned _endIndex;
    unsigned _cacheSizeB[3]; // [0] = L1, [1] = L2, [2] = L3
    unsigned _cacheLines[3]; // [0] = L1, [1] = L2, [2] = L3
    unsigned _elementsPerCacheLine;
        //CPU is the processor number
    //static constexpr inline load
    
    static unsigned _calculateCacheLines(unsigned &cacheSizeB){
        return cacheSizeB / 64;
    }
        

};