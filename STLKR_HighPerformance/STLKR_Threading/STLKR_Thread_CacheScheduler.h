//
// Created by hal9000 on 6/8/24.
//

#ifndef STALKER_STLKR_THREAD_CACHESCHEDULER_H
#define STALKER_STLKR_THREAD_CACHESCHEDULER_H

#include <immintrin.h>
#include <iostream>
#include "STLKR_Thread_CacheLineInstance.h"
#include "../STKR_MachineSetup/STLKR_Machine_CPUTopologyLinux.h"
template <typename T, unsigned physicalCores>
class STLKR_Thread_CacheScheduler {

public:
    STLKR_Thread_CacheScheduler(unsigned size, unsigned L1CacheSizeKB, unsigned L2CacheSizeKB, unsigned L3CacheSizeKB, unsigned cacheLineSize = 64){
        _size = size; 
        _physicalCoreBlockSize = _size / physicalCores;
        auto cacheSizeL1 = _convertToBytes(L1CacheSizeKB);
        auto cacheSizeL2 = _convertToBytes(L2CacheSizeKB);
        auto cacheSizeL3 = _convertToBytes(L3CacheSizeKB);
        unsigned startIndex, endIndex;
        for (int i = 0; i < physicalCores; ++i) {
            std::cout << "Core: " << i << std::endl;
            startIndex = i * _physicalCoreBlockSize;
            endIndex = (i + 1 == physicalCores ? _size : (i + 1) * _physicalCoreBlockSize);
            _cacheLineInstances[i] = STLKR_Thread_CacheLineInstance<T>(i, startIndex, endIndex,
                                                                          cacheSizeL1,cacheSizeL2, cacheSizeL3, cacheLineSize);
        }
        
        auto systemProcessorReader = STLKR_Machine_CPUTopologyLinux();
        systemProcessorReader.print_processor_specs();
        auto lol =1;
        
    }
       
private:
    unsigned _size;
    unsigned _physicalCoreBlockSize;
    unsigned _totalCacheLines;
    STLKR_Thread_CacheLineInstance<T> _cacheLineInstances[physicalCores];
    
    static unsigned _convertToBytes(unsigned& sizeKB){
        return sizeKB *= 1024;
    }
};

#endif //STALKER_STLKR_THREAD_CACHESCHEDULER_H
