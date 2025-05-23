//
// Created by hal9000 on 7/30/24.
//

#ifndef STALKER_PREFETCH_OPERATIONS_H
#define STALKER_PREFETCH_OPERATIONS_H
#include <mmintrin.h>
#include "MemoryTraits.h"

enum PrefetchConfig {
    PrefetchToL1, // Corresponds to _MM_HINT_T0: Prefetch to L1 cache (and all levels below it)
    PrefetchToL2, // Corresponds to _MM_HINT_T1: Prefetch to L2 cache (and all levels below it)
    PrefetchToL3  // Corresponds to _MM_HINT_T2: Prefetch to L3 cache (and all levels below it)
};

class Prefetch_Operations {
public:
    template<typename T, unsigned numVectors>
    static constexpr inline void fillCacheLevel(T** data, unsigned cacheSizeKB, double cacheThreshold, PrefetchConfig prefetchConfig) {
        if (cacheThreshold < 0 || cacheThreshold > 1)
            cacheThreshold = 0.8;
        auto elementsPerCacheLine = 64 / sizeof(T);
        auto cacheSizeInCacheLines = static_cast<unsigned>(cacheThreshold * cacheSizeKB * 1024 / 64);
        auto cacheLinesToPrefetch = cacheSizeInCacheLines / numVectors;
        switch (prefetchConfig) {
            case PrefetchToL1:
                _prefetch_L1<T, numVectors>(data, elementsPerCacheLine, cacheLinesToPrefetch);
                break;
            case PrefetchToL2:
                _prefetch_L2<T, numVectors>(data, elementsPerCacheLine, cacheLinesToPrefetch);
                break;
            case PrefetchToL3:
                _prefetch_L3<T, numVectors>(data, elementsPerCacheLine, cacheLinesToPrefetch);
                break;
        }
    }

    template<typename T, unsigned numVectors>
    static constexpr inline void prefetchCacheLines(T** data, unsigned cacheSize, double cacheThreshold, PrefetchConfig prefetchConfig, unsigned numOfCacheLines) {
        if (cacheThreshold < 0 || cacheThreshold > 1)
            cacheThreshold = 0.8;
        auto elementsPerCacheLine = 64 / sizeof(T);
        switch (prefetchConfig) {
            case PrefetchToL1:
                _prefetch_L1<T, numVectors>(data, elementsPerCacheLine, numOfCacheLines);
                break;
            case PrefetchToL2:
                _prefetch_L2<T, numVectors>(data, elementsPerCacheLine, numOfCacheLines);
                break;
            case PrefetchToL3:
                _prefetch_L3<T, numVectors>(data, elementsPerCacheLine, numOfCacheLines);
                break;
        }
    }

private:
    template<typename T, unsigned iVector>
    static constexpr inline void _prefetch_L1(T** data, unsigned elementsPerCacheLine, unsigned cacheLinesToPrefetch) {
        if constexpr (iVector > 0) {
            for (unsigned iCacheLine = 0; iCacheLine < cacheLinesToPrefetch; ++iCacheLine)
                _mm_prefetch(reinterpret_cast<const char*>(data[iVector - 1] + iCacheLine * elementsPerCacheLine), _MM_HINT_T0);
            _prefetch_L1<T, iVector - 1>(data, elementsPerCacheLine, cacheLinesToPrefetch);
        }
    }

    template<typename T, unsigned iVector>
    static constexpr inline void _prefetch_L2(T** data, unsigned elementsPerCacheLine, unsigned cacheLinesToPrefetch) {
        if constexpr (iVector > 0) {
            for (unsigned iCacheLine = 0; iCacheLine < cacheLinesToPrefetch; ++iCacheLine)
                _mm_prefetch(reinterpret_cast<const char*>(data[iVector - 1] + iCacheLine * elementsPerCacheLine), _MM_HINT_T1);
            _prefetch_L2<T, iVector - 1>(data, elementsPerCacheLine, cacheLinesToPrefetch);
        }
    }

    template<typename T, unsigned iVector>
    static constexpr inline void _prefetch_L3(T** data, unsigned elementsPerCacheLine, unsigned cacheLinesToPrefetch) {
        if constexpr (iVector > 0) {
            for (unsigned iCacheLine = 0; iCacheLine < cacheLinesToPrefetch; ++iCacheLine)
                _mm_prefetch(reinterpret_cast<const char*>(data[iVector - 1] + iCacheLine * elementsPerCacheLine), _MM_HINT_T2);
            _prefetch_L3<T, iVector - 1>(data, elementsPerCacheLine, cacheLinesToPrefetch);
        }
    }
};



#endif //STALKER_PREFETCH_OPERATIONS_H
