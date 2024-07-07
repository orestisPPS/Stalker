//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_AVX_OPERATIONS_H
#define STALKER_AVX_OPERATIONS_H
#include <immintrin.h>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>
#include <iomanip>
#include "AVX_MemoryManagement.h"
#include "Thread_Operations.h"

template <typename T, unsigned unrollFactor, size_t availableThreads = 1>
class AVX_Operations {

 
public:

    constexpr inline static void deepcopy(T *destination, T  *source, unsigned size, bool temporalStore = false) {

        auto avxTraits = AVX_Traits<T, unrollFactor>();
        using dataType = typename AVX_Traits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_Traits<T, unrollFactor>::AVXRegister;
        
        
        avxRegisterType simdData[unrollFactor];
        void (*storeResultRegister)(avxRegisterType*, dataType*) = temporalStore ?
            AVX_MemoryManagement::storeTemporalData<dataType, avxRegisterType, unrollFactor> :
            AVX_MemoryManagement::storeNonTemporalData<dataType, avxRegisterType, unrollFactor>;
        
        auto registerSize = avxTraits.AVXRegisterSize;
        auto limit = size - (size % registerSize);
        
        for (size_t i = 0; i < limit; i += registerSize * unrollFactor) {
            AVX_MemoryManagement::loadDataRegister<dataType, avxRegisterType, unrollFactor>(source + i, simdData);
            storeResultRegister(simdData, destination + i);
        }
        
        for (size_t i = limit; i < size; i++) {
            destination[i] = source[i];
        }
    }
    
//    constexpr static void add(double *data1, double scale1, double *data2, double scale2, double *result, unsigned size, AVX_Config prefetchConfig) {
//        size_t limit = size - (size % (DOUBLE_AVX_REGISTER_SIZE * 4));  // Adjusted for unrolling factor
//        __m256d va1, va2, va3, va4, ve1, ve2, ve3, ve4, vb1, vb2, vb3, vb4;
//
//        __m256d vScale1 = _mm256_set1_pd(scale1);
//        __m256d vScale2 = _mm256_set1_pd(scale2);
//        auto prefetchDistance = prefetchConfig.getDistance();
//        auto hint = prefetchConfig.getHint();
//        auto storeType = prefetchConfig.getStore();
//
////        auto storeFunc = (storeType == STLKR_SIMD_Stores::Regular) ?
////                         [](double* dest, __m256d val) { _mm256_store_pd(dest, val); } :
////                         [](double* dest, __m256d val) { _mm256_stream_pd(dest, val); };
//
//        for (size_t i = 0; i < limit; i += 4
//                ) {  // Adjust loop increment based on unrolling factor
//            // Prefetch data that will be needed soon
//            if (i + prefetchDistance < size) {
////                    _mm_prefetch((const char*)(data1 + i + prefetchDistance), hint);
////                    _mm_prefetch((const char*)(data2 + i + prefetchDistance), hint);
////                    _mm_prefetch((const char*)(result + i + prefetchDistance), hint);
//            }
//
//            va1 = _mm256_load_pd(data1 + i);
//            //va2 = _mm256_load_pd(data1 + i + 4);
//            //va3 = _mm256_load_pd(data1 + i + 8);
//            //va4 = _mm256_load_pd(data1 + i + 12);
//            vb1 = _mm256_load_pd(data2 + i);
//            //vb2 = _mm256_load_pd(data2 + i + 4);
//            //vb3 = _mm256_load_pd(data2 + i + 8);
//            //vb4 = _mm256_load_pd(data2 + i + 12);
//
//            ve1 = _mm256_fmadd_pd(va1, vScale1, _mm256_mul_pd(vb1, vScale2));
//            //ve2 = _mm256_fmadd_pd(va2, vScale1, _mm256_mul_pd(vb2, vScale2));
//            //ve3 = _mm256_fmadd_pd(va3, vScale1, _mm256_mul_pd(vb3, vScale2));
//            //ve4 = _mm256_fmadd_pd(va4, vScale1, _mm256_mul_pd(vb4, vScale2));
//
//        }
//
//        for (size_t i = limit; i < size; i++) {
//            result[i] = data1[i] * scale1 + data2[i] * scale2;
//        }
//    }
//    template<unsigned int numVectors>
//    static constexpr inline void _addSIMD(double** data, double* scaleFactors, double* result, unsigned int size, bool temporalStore = false) {
//        __m256d simdData[numVectors * unrollFactor];
//        __m256d simdScalars[numVectors];
//        __m256d simdResult[unrollFactor];
//        void (*storeResultRegister)(__m256d*, double*) = temporalStore ?
//            AVX_MemoryManagement::storeTemporalData<double, __m256d, unrollFactor> :
//            AVX_MemoryManagement::storeNonTemporalData<double, __m256d, unrollFactor>;
//        size_t limit = size - (size % (DOUBLE_AVX_REGISTER_SIZE * unrollFactor));
//
//        AVX_MemoryManagement::broadcastScalars<double,__m256d, numVectors>(scaleFactors, simdScalars);
//        for (size_t i = 0; i < limit; i += DOUBLE_AVX_REGISTER_SIZE * unrollFactor) {
//            AVX_MemoryManagement::loadMultipleDataRegisters<double, __m256d, unrollFactor, numVectors>(data, simdData, unrollFactor, i);
//            _fusedMultiplyAddDoubleVectors<numVectors>(simdData, simdScalars, simdResult);
//            storeResultRegister(simdResult, result + i);
//            AVX_MemoryManagement::setZero<double, __m256d, unrollFactor>(simdResult);
//        }
//        for (size_t i = limit; i < size; i++) {
//            for (size_t j = 0; j < numVectors; j++) {
//                result[i] += data[j][i] * scaleFactors[j];
//            }
//        }
//
//    }
//
//    template<unsigned int numVectors, unsigned numPhysicalCores>
//    static constexpr inline void add(double** data, double* scaleFactors, double* result, unsigned int size, bool temporalStore = false) {
//
//        __m256d simdScalars[numVectors];
//        AVX_MemoryManagement::broadcastScalars<double,__m256d, numVectors>(scaleFactors, simdScalars);
//        
//        std::pair<unsigned, unsigned> threadLimits[numPhysicalCores];
//        unsigned simdBlockSize = DOUBLE_AVX_REGISTER_SIZE * unrollFactor;
//        unsigned totalSimdBlocks = (size + simdBlockSize - 1) / simdBlockSize;
//        unsigned threadBlockSize = (totalSimdBlocks + numPhysicalCores - 1) / numPhysicalCores;
//        unsigned startBlock = 0, endBlock = 0;
//        for (size_t i = 0; i < numPhysicalCores; i++) {
//            startBlock = i * threadBlockSize;
//            endBlock = std::min(startBlock + threadBlockSize, totalSimdBlocks);
//            threadLimits[i].first = startBlock * simdBlockSize;
//            threadLimits[i].second = std::min(endBlock * simdBlockSize, size);
//        }
//        
//        auto addThreadJob = [&](unsigned startIndex, unsigned endIndex) {
//            __m256d simdData[numVectors * unrollFactor];
//            __m256d simdResult[unrollFactor];
//            void (*storeResultRegister)(__m256d*, double*) = temporalStore ?
//                 AVX_MemoryManagement::storeTemporalData<double, __m256d, unrollFactor> :
//                 AVX_MemoryManagement::storeNonTemporalData<double, __m256d, unrollFactor>;
//            
//            size_t limit = endIndex - (endIndex % (DOUBLE_AVX_REGISTER_SIZE * unrollFactor));
//            for (size_t i = startIndex; i < limit; i += DOUBLE_AVX_REGISTER_SIZE * unrollFactor) {
//                AVX_MemoryManagement::loadMultipleDataRegisters<double, __m256d, unrollFactor, numVectors>(data, simdData, unrollFactor, i);
//                _fusedMultiplyAddDoubleVectors<numVectors>(simdData, simdScalars, simdResult);
//                storeResultRegister(simdResult, result + i);
//                AVX_MemoryManagement::setZero<double, __m256d, unrollFactor>(simdResult);
//            }
//        };
//        
//        Thread_Operations::executeJob(addThreadJob, size, threadLimits);
//        for (size_t i = threadLimits[numPhysicalCores - 1].second; i < size; i++) {
//            for (size_t j = 0; j < numVectors; j++) {
//                result[i] += data[j][i] * scaleFactors[j];
//            }
//        }
//    }
    
    

private:
    template<size_t iUnroll>
    static constexpr inline void _deepCopy(const __m256d *simdData) {
        if constexpr (iUnroll > 0) {
            
            _deepCopy<iUnroll - 1>(simdData);
        }
        else return;
    }

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
#endif //STALKER_AVX_OPERATIONS_H
 