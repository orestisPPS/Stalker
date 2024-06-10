//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_STLKR_OPERATIONS_SIMD_H
#define STALKER_STLKR_OPERATIONS_SIMD_H
#include <immintrin.h>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>
#include <iomanip>
#include "STLKR_SIMD_MemoryManager.h"

#include "../STLKR_Threading/STLKR_Thread_Operations.h"
#include "STLKR_Config_SIMD.h"
#include "../STLKR_Threading/STLKR_Thread_CacheScheduler.h"

template <unsigned unrollFactor, size_t availableThreads = 1>
class STLKR_Operations_SIMD {

public:
    
//    constexpr inline static void deepcopy(double *destination, double  *source, unsigned size) {
//        auto registerSize = STLKR_SIMD_DataTypeSize.at(type_index(typeid(T)));
//        auto limit = size - (size % AVX_DOUBLE_SIZE);
//        
//        if constexpr (is_same_v<T, double>) {
//            for (unsigned i = 0; i < limit; i += AVX_DOUBLE_SIZE * 4) {
//                __m256d va1 = _mm256_load_pd(source + i);
//                __m256d va2 = _mm256_load_pd(source + i + AVX_DOUBLE_SIZE);
//                __m256d va3 = _mm256_load_pd(source + i + 2 * AVX_DOUBLE_SIZE);
//                __m256d va4 = _mm256_load_pd(source + i + 3 * AVX_DOUBLE_SIZE);
//                _mm256_store_pd(destination + i, va1);
//                _mm256_store_pd(destination + i + AVX_DOUBLE_SIZE, va2);
//                _mm256_store_pd(destination + i + 2 * AVX_DOUBLE_SIZE, va3);
//                _mm256_store_pd(destination + i + 3 * AVX_DOUBLE_SIZE, va4);
//            }
//        }
//        else if constexpr (is_same_v<T, double>) {
//            for (unsigned i = 0; i < size - (size % AVX_DOUBLE_SIZE); i += AVX_DOUBLE_SIZE) {
//                __m256d va = _mm256_load_pd(source + i);
//                _mm256_store_pd(destination + i, va);
//            }
//        } else if constexpr (is_same_v<T, int> || is_same_v<T, unsigned> || is_same_v<T, short>) {
//            for (unsigned i = 0; i < size - (size % AVX_DOUBLE_SIZE); i += AVX_DOUBLE_SIZE) {
//                __m256i va = _mm256_loadu_si256((__m256i *)(source + i));
//                _mm256_storeu_si256((__m256i *)(destination + i), va);
//            }
//        } else {
//            throw runtime_error("T must be float, double, int, unsigned, or short");
//        }
//    }


    constexpr static void add(double *data1, double scale1, double *data2, double scale2, double *result, unsigned size,STLKR_Config_SIMD prefetchConfig) {
        size_t limit = size - (size % (DOUBLE_AVX_REGISTER_SIZE * 4));  // Adjusted for unrolling factor
        __m256d va1, va2, va3, va4, ve1, ve2, ve3, ve4, vb1, vb2, vb3, vb4;

        __m256d vScale1 = _mm256_set1_pd(scale1);
        __m256d vScale2 = _mm256_set1_pd(scale2);
        auto prefetchDistance = prefetchConfig.getDistance();
        auto hint = prefetchConfig.getHint();
        auto storeType = prefetchConfig.getStore();

//        auto storeFunc = (storeType == STLKR_SIMD_Stores::Regular) ?
//                         [](double* dest, __m256d val) { _mm256_store_pd(dest, val); } :
//                         [](double* dest, __m256d val) { _mm256_stream_pd(dest, val); };

        for (size_t i = 0; i < limit; i += 4
                ) {  // Adjust loop increment based on unrolling factor
            // Prefetch data that will be needed soon
            if (i + prefetchDistance < size) {
//                    _mm_prefetch((const char*)(data1 + i + prefetchDistance), hint);
//                    _mm_prefetch((const char*)(data2 + i + prefetchDistance), hint);
//                    _mm_prefetch((const char*)(result + i + prefetchDistance), hint);
            }

            va1 = _mm256_load_pd(data1 + i);
            //va2 = _mm256_load_pd(data1 + i + 4);
            //va3 = _mm256_load_pd(data1 + i + 8);
            //va4 = _mm256_load_pd(data1 + i + 12);
            vb1 = _mm256_load_pd(data2 + i);
            //vb2 = _mm256_load_pd(data2 + i + 4);
            //vb3 = _mm256_load_pd(data2 + i + 8);
            //vb4 = _mm256_load_pd(data2 + i + 12);

            ve1 = _mm256_fmadd_pd(va1, vScale1, _mm256_mul_pd(vb1, vScale2));
            //ve2 = _mm256_fmadd_pd(va2, vScale1, _mm256_mul_pd(vb2, vScale2));
            //ve3 = _mm256_fmadd_pd(va3, vScale1, _mm256_mul_pd(vb3, vScale2));
            //ve4 = _mm256_fmadd_pd(va4, vScale1, _mm256_mul_pd(vb4, vScale2));

        }

        for (size_t i = limit; i < size; i++) {
            result[i] = data1[i] * scale1 + data2[i] * scale2;
        }
    }
    template<unsigned int numVectors>
    static constexpr inline void _addSIMD(double** data, double* scaleFactors, double* result, unsigned int size, STLKR_Config_SIMD prefetchConfig) {
        __m256d simdData[numVectors * unrollFactor];
        __m256d simdScalars[numVectors];
        __m256d simdResult[unrollFactor];
        void (*storeResultRegister)(__m256d*, double*) = (prefetchConfig.getStore() == STLKR_SIMD_Stores::Temporal) ?
            STLKR_SIMD_MemoryManager::storeTemporalData<double, __m256d, unrollFactor> :
            STLKR_SIMD_MemoryManager::storeNonTemporalData<double, __m256d, unrollFactor>;
        size_t limit = size - (size % (DOUBLE_AVX_REGISTER_SIZE * unrollFactor));

        STLKR_SIMD_MemoryManager::broadcastScalars<double,__m256d, numVectors>(scaleFactors, simdScalars);
        for (size_t i = 0; i < limit; i += DOUBLE_AVX_REGISTER_SIZE * unrollFactor) {
            STLKR_SIMD_MemoryManager::loadMultipleDataRegisters<double, __m256d, unrollFactor, numVectors>(data, simdData, unrollFactor, i);
            _fusedMultiplyAddDoubleVectors<numVectors>(simdData, simdScalars, simdResult);
            storeResultRegister(simdResult, result + i);
            STLKR_SIMD_MemoryManager::setZero<double, __m256d, unrollFactor>(simdResult);
        }
        for (size_t i = limit; i < size; i++) {
            for (size_t j = 0; j < numVectors; j++) {
                result[i] += data[j][i] * scaleFactors[j];
            }
        }

        auto cacheScheduler = STLKR_Thread_CacheScheduler<double, availableThreads>(size, 32, 256, 1024);
    }

private:


    template<size_t iUnroll>
    static constexpr inline void _fusedMultiplyAddDoubles(const __m256d *simdData, const __m256d *scale, __m256d *result) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_fmadd_pd(*(simdData + iUnroll - 1), *scale, *(result + iUnroll - 1));
            _fusedMultiplyAddDoubles<iUnroll - 1>(simdData, scale, result);
        }
        else return;
    }

    template<size_t iVector>
    static constexpr inline void _fusedMultiplyAddDoubleVectors(const __m256d *simdData,  const __m256d *scale, __m256d *result) {
        if constexpr (iVector > 0) {
            _fusedMultiplyAddDoubles<unrollFactor>(simdData + (iVector - 1) * unrollFactor, scale + (iVector - 1), result);
            _fusedMultiplyAddDoubleVectors<iVector - 1>(simdData, scale, result);
        }
        else return;
    }
    
    
};// STLKR_LinearAlgebra
#endif //STALKER_STLKR_OPERATIONS_SIMD_H
 