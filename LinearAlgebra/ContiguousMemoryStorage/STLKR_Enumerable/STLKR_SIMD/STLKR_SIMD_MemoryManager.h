//
// Created by hal9000 on 6/7/24.
//

#ifndef STALKER_STLKR_SIMD_MEMORYMANAGER_H
#define STALKER_STLKR_SIMD_MEMORYMANAGER_H

#define AVX_DOUBLE_SIZE 4
#define AVX_FLOAT_SIZE 8
#define AVX_INT_SIZE 8
#define AVX_SHORT_SIZE 16
#define AVX_UNSIGNED_SIZE 8

#include <cstdlib>
#include <immintrin.h>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <typeindex>

#include "STLKR_Config_SIMD.h"

class STLKR_SIMD_MemoryManager {

//    using PrefetchFunction = std::function<void(T*)>;
//
//    using StoreDoubleFunction = std::function<void(__m256d*, T*)>;
    
    public:

    const std::unordered_map<std::type_index, unsigned> STLKR_SIMD_DataTypeSize = {
            {std::type_index(typeid(float)), AVX_FLOAT_SIZE},
            {std::type_index(typeid(double)), AVX_DOUBLE_SIZE},
            {std::type_index(typeid(int)), AVX_INT_SIZE},
            {std::type_index(typeid(short)), AVX_SHORT_SIZE},
            {std::type_index(typeid(unsigned)), AVX_UNSIGNED_SIZE}
    };

    template <typename T>
    static constexpr inline void allocate(unsigned int size, unsigned int alignment) {
        aligned_alloc(alignment, size * sizeof(T));
    }

    template <typename T>
    static constexpr inline void deallocate(T *ptr) {
        free(ptr);
    }


    template <typename T>
    static constexpr inline void prefetch(T *data, _mm_hint hint) {
        switch (hint) {
            case _MM_HINT_T0:
                _prefetchCacheLine_T0(data);
                break;
            case _MM_HINT_T1:
                _prefetchCacheLine_T1(data);
                break;
            case _MM_HINT_T2:
                _prefetchCacheLine_T2(data);
                break;
            case _MM_HINT_NTA:
                _prefetchCacheLine_NTA(data);
                break;
            default:
                throw std::invalid_argument("Invalid hint");
        }
    }

    static void loadDoubleRegister(__m256d *avxData, const double *data) {
        *avxData = _mm256_load_pd(data);
    }

    template <unsigned int iUnroll>
    static constexpr inline void loadDoublesUnrolled(__m256d *avxData, double *data) {
        if constexpr (iUnroll > 0) {
            *(avxData + iUnroll - 1) = _mm256_load_pd(data + (iUnroll - 1) * AVX_DOUBLE_SIZE);
            loadDoublesUnrolled<iUnroll - 1>(avxData, data);
        }
        else return;
    }


    template <unsigned int iUnroll, unsigned int iVector>
    static constexpr inline void loadVectorsOfDoublesUnrolled(__m256d *avxData, double **data,
                                                                          unsigned int unrollFactor, unsigned int index) {
        if constexpr (iVector > 0) {
            loadDoublesUnrolled<iUnroll>(avxData + (iVector - 1) * unrollFactor, *(data + iVector - 1) + index);
            loadVectorsOfDoublesUnrolled<iUnroll, iVector - 1>(avxData, data, unrollFactor, index);
        }
        else return;
    }
    
    private:

    template <typename T>
    static constexpr inline void _prefetchCacheLine_T0(T *data) {
        _mm_prefetch(reinterpret_cast<const char*>(data), _MM_HINT_T0);
    }

    template <typename T>
    static constexpr inline void _prefetchCacheLine_T1(T *data) {
        _mm_prefetch(reinterpret_cast<const char*>(data), _MM_HINT_T1);
    }

    template <typename T>
    static constexpr inline void _prefetchCacheLine_T2(T *data) {
        _mm_prefetch(reinterpret_cast<const char*>(data), _MM_HINT_T2);
    }

    template <typename T>
    static constexpr inline void _prefetchCacheLine_NTA(T *data) {
        _mm_prefetch(reinterpret_cast<const char*>(data), _MM_HINT_NTA);
    }

//constexpr void _storeDoubleTemporal(__m256d *src, double *dest) {
//    _mm256_stream_pd(dest, *src);
//}
//
//constexpr void _storeDoubleNonTemporal(__m256d *src, double *dest) {
//    _mm256_store_pd(dest, *src);
//}

    
    
    
    
};


#endif //STALKER_STLKR_SIMD_MEMORYMANAGER_H
