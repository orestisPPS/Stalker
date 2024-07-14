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
#include <complex>
#include "AVX_MemoryManagement.h"
#include "AVX_MathTraits.h"
#include "Thread_Operations.h"

template <typename T, unsigned unrollFactor, size_t availableThreads = 1>
class AVX_Operations {

 
public:
    

    constexpr inline static void copy(T *destination, T  *source, unsigned size, unsigned cores, CPU_Manager &manager, bool temporalStore = false) {
        
        auto avxTraits = AVX_MemoryTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;
        
        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxTraits.AVXRegisterSize * unrollFactor;
        auto totalBlocks = (size + blockSize - 1) / blockSize;
        auto threadBlockSize = (totalBlocks + cores - 1) / cores;
        auto startBlock = 0, endBlock = 0;
        for (size_t i = 0; i < cores; i++) {
            startBlock = i * threadBlockSize;
            endBlock = std::min(startBlock + threadBlockSize, totalBlocks);
            threadLimits[i].first = startBlock * blockSize;
            threadLimits[i].second = std::min(endBlock * blockSize, size);
        }
        
        auto deepcopyThreadJob = [&](unsigned startIndex, unsigned endIndex) {
            avxRegisterType simdData[unrollFactor];
            void (*storeResultRegister)(const avxRegisterType*, dataType*) = temporalStore ?
                avxTraits.storeAVXRegisterTemporal : avxTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxTraits.loadAVXRegister(source + i, simdData);
                storeResultRegister(simdData, destination + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                destination[i] = source[i];
            }
        };
        Thread_Operations::executeJob(deepcopyThreadJob, size, cores, threadLimits, manager);
        
        delete[] threadLimits;
    }

    constexpr inline static void setValue(T *destination, T  value, unsigned size, unsigned cores, CPU_Manager &manager, bool temporalStore = false) {

        auto avxTraits = AVX_MemoryTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxTraits.AVXRegisterSize * unrollFactor;
        auto totalBlocks = (size + blockSize - 1) / blockSize;
        auto threadBlockSize = (totalBlocks + cores - 1) / cores;
        auto startBlock = 0, endBlock = 0;
        for (size_t i = 0; i < cores; i++) {
            startBlock = i * threadBlockSize;
            endBlock = std::min(startBlock + threadBlockSize, totalBlocks);
            threadLimits[i].first = startBlock * blockSize;
            threadLimits[i].second = std::min(endBlock * blockSize, size);
        }
        
        auto setValueThreadJob = [&](unsigned startIndex, unsigned endIndex) {
            avxRegisterType simdData[unrollFactor];
            void (*storeResultRegister)(const avxRegisterType*, dataType*) = temporalStore ?
                avxTraits.storeAVXRegisterTemporal : avxTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxTraits.setValue(simdData, value);
                storeResultRegister(simdData, destination + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                destination[i] = value;
            }
        };
        
        Thread_Operations::executeJob(setValueThreadJob, size, cores, threadLimits, manager);
    }
    
    constexpr inline static bool areEqual(const T *a, const T *b, unsigned size, unsigned cores, CPU_Manager &manager){
        auto avxTraits = AVX_MemoryTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxTraits.AVXRegisterSize * unrollFactor;
        auto totalBlocks = (size + blockSize - 1) / blockSize;
        auto threadBlockSize = (totalBlocks + cores - 1) / cores;
        auto startBlock = 0, endBlock = 0;
        for (size_t i = 0; i < cores; i++) {
            startBlock = i * threadBlockSize;
            endBlock = std::min(startBlock + threadBlockSize, totalBlocks);
            threadLimits[i].first = startBlock * blockSize;
            threadLimits[i].second = std::min(endBlock * blockSize, size);
        }
        bool result = true;
        auto areEqualThreadJob = [&](unsigned startIndex, unsigned endIndex) {
            avxRegisterType simdDataA[unrollFactor];
            avxRegisterType simdDataB[unrollFactor];
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxTraits.loadAVXRegister(a + i, simdDataA);
                avxTraits.loadAVXRegister(b + i, simdDataB);
                result = avxTraits.areEqual(simdDataA, simdDataB);
                if (result == false)
                    return;
            }
            for (size_t i = limit; i < endIndex; i++) {
                if (a[i] != b[i]) {
                    result = false;
                    return;
                }
            }
        };
        Thread_Operations::executeJob(areEqualThreadJob, size, cores, threadLimits, manager);
        return result;
    }
    
    constexpr inline static void add(T *data1, T  *data2, T *result, T scale1, T scale2, unsigned size, unsigned cores, CPU_Manager &manager, bool temporalStore = false) {

        auto avxTraits = AVX_MemoryTraits<T, unrollFactor>();
        auto avxMathTraits = AVX_MathTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxTraits.AVXRegisterSize * unrollFactor;
        auto totalBlocks = (size + blockSize - 1) / blockSize;
        auto threadBlockSize = (totalBlocks + cores - 1) / cores;
        auto startBlock = 0, endBlock = 0;
        for (size_t i = 0; i < cores; i++) {
            startBlock = i * threadBlockSize;
            endBlock = std::min(startBlock + threadBlockSize, totalBlocks);
            threadLimits[i].first = startBlock * blockSize;
            threadLimits[i].second = std::min(endBlock * blockSize, size);
        }
        avxRegisterType simdScale1, simdScale2;
        avxTraits.setValue(&simdScale1, scale1);
        avxTraits.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex) {
            avxRegisterType simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxRegisterType*, dataType*) = temporalStore ?
                    avxTraits.storeAVXRegisterTemporal : avxTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxTraits.loadAVXRegister(data1 + i, simdData1);
                avxTraits.loadAVXRegister(data2 + i, simdData2);
                avxTraits.loadAVXRegister(result + i, simdResult);
                avxMathTraits.add(simdData1, simdData2, simdResult, &simdScale1, &simdScale2);
                storeResultRegister(simdResult, result + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                result[i] = data1[i] * scale1 + data2[i] * scale2;
            }
        };
        Thread_Operations::executeJob(addJob, size, cores, threadLimits, manager);
        delete[] threadLimits;
    }

    constexpr inline static void subtract(T *data1, T  *data2, T *result, T scale1, T scale2, unsigned size, unsigned cores, CPU_Manager &manager, bool temporalStore = false) {

        auto avxTraits = AVX_MemoryTraits<T, unrollFactor>();
        auto avxMathTraits = AVX_MathTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxTraits.AVXRegisterSize * unrollFactor;
        auto totalBlocks = (size + blockSize - 1) / blockSize;
        auto threadBlockSize = (totalBlocks + cores - 1) / cores;
        auto startBlock = 0, endBlock = 0;
        for (size_t i = 0; i < cores; i++) {
            startBlock = i * threadBlockSize;
            endBlock = std::min(startBlock + threadBlockSize, totalBlocks);
            threadLimits[i].first = startBlock * blockSize;
            threadLimits[i].second = std::min(endBlock * blockSize, size);
        }
        avxRegisterType simdScale1, simdScale2;
        avxTraits.setValue(&simdScale1, scale1);
        avxTraits.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex) {
            avxRegisterType simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxRegisterType*, dataType*) = temporalStore ?
                                                                             avxTraits.storeAVXRegisterTemporal : avxTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxTraits.loadAVXRegister(data1 + i, simdData1);
                avxTraits.loadAVXRegister(data2 + i, simdData2);
                avxTraits.loadAVXRegister(result + i, simdResult);
                avxMathTraits.subtract(simdData1, simdData2, simdResult, &simdScale1, &simdScale2);
                storeResultRegister(simdResult, result + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                result[i] = data1[i] * scale1 - data2[i] * scale2;
            }
        };
        Thread_Operations::executeJob(addJob, size, cores, threadLimits, manager);
        delete[] threadLimits;
    }


    constexpr inline static void multiply(T *data1, T  *data2, T *result, T scale1, T scale2, unsigned size, unsigned cores, CPU_Manager &manager, bool temporalStore = false) {

        auto avxTraits = AVX_MemoryTraits<T, unrollFactor>();
        auto avxMathTraits = AVX_MathTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxTraits.AVXRegisterSize * unrollFactor;
        auto totalBlocks = (size + blockSize - 1) / blockSize;
        auto threadBlockSize = (totalBlocks + cores - 1) / cores;
        auto startBlock = 0, endBlock = 0;
        for (size_t i = 0; i < cores; i++) {
            startBlock = i * threadBlockSize;
            endBlock = std::min(startBlock + threadBlockSize, totalBlocks);
            threadLimits[i].first = startBlock * blockSize;
            threadLimits[i].second = std::min(endBlock * blockSize, size);
        }
        avxRegisterType simdScale1, simdScale2;
        avxTraits.setValue(&simdScale1, scale1);
        avxTraits.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex) {
            avxRegisterType simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxRegisterType*, dataType*) = temporalStore ?
                                                                             avxTraits.storeAVXRegisterTemporal : avxTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxTraits.loadAVXRegister(data1 + i, simdData1);
                avxTraits.loadAVXRegister(data2 + i, simdData2);
                avxTraits.loadAVXRegister(result + i, simdResult);
                avxMathTraits.multiply(simdData1, simdData2, simdResult, &simdScale1, &simdScale2);
                storeResultRegister(simdResult, result + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                result[i] = data1[i] * scale1 * data2[i] * scale2;
            }
        };
        Thread_Operations::executeJob(addJob, size, cores, threadLimits, manager);
        delete[] threadLimits;
    }
    
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
 