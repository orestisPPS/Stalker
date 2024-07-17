//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_AVX_OPERATIONS_H
#define STALKER_AVX_OPERATIONS_H
#include "AVX_MemoryManagement.h"
#include "AVX_MathTraits.h"
#include "Thread_Operations.h"

template <typename T, unsigned unrollFactor>
class AVX_Operations {
    
public:

    constexpr inline static void copy(T *destination, T  *source, unsigned size, unsigned cores, CPU_Manager &manager, bool temporalStore = false) {
        
        auto avxMemoryTraits = AVX_MemoryTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;
        
        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxMemoryTraits.AVXRegisterSize * unrollFactor;
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
                avxMemoryTraits.storeAVXRegisterTemporal : avxMemoryTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxMemoryTraits.loadAVXRegister(source + i, simdData);
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

        auto avxMemoryTraits = AVX_MemoryTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxMemoryTraits.AVXRegisterSize * unrollFactor;
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
                avxMemoryTraits.storeAVXRegisterTemporal : avxMemoryTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxMemoryTraits.setValue(simdData, value);
                storeResultRegister(simdData, destination + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                destination[i] = value;
            }
        };
        
        Thread_Operations::executeJob(setValueThreadJob, size, cores, threadLimits, manager);
    }
    
    constexpr inline static void scale(T *data, T  scale, unsigned size, unsigned cores, CPU_Manager &manager, bool temporalStore = false) {

        auto avxMemoryTraits = AVX_MemoryTraits<T, unrollFactor>();
        auto avxMathTraits = AVX_MathTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxMemoryTraits.AVXRegisterSize * unrollFactor;
        auto totalBlocks = (size + blockSize - 1) / blockSize;
        auto threadBlockSize = (totalBlocks + cores - 1) / cores;
        auto startBlock = 0, endBlock = 0;
        for (size_t i = 0; i < cores; i++) {
            startBlock = i * threadBlockSize;
            endBlock = std::min(startBlock + threadBlockSize, totalBlocks);
            threadLimits[i].first = startBlock * blockSize;
            threadLimits[i].second = std::min(endBlock * blockSize, size);
        }
        avxRegisterType simdScale;
        avxMemoryTraits.setValue(&simdScale, scale);
        auto scaleThreadJob = [&](unsigned startIndex, unsigned endIndex) {
            avxRegisterType simdData[unrollFactor];
            void (*storeResultRegister)(const avxRegisterType*, dataType*) = temporalStore ?
                avxMemoryTraits.storeAVXRegisterTemporal : avxMemoryTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxMemoryTraits.loadAVXRegister(data + i, simdData);
                avxMathTraits.scale(simdData, &simdScale);
                storeResultRegister(simdData, data + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                data[i] *= scale;
            }
        };
        Thread_Operations::executeJob(scaleThreadJob, size, cores, threadLimits, manager);
        delete[] threadLimits;
    }
    
    constexpr inline static bool areEqual(const T *a, const T *b, unsigned size, unsigned cores, CPU_Manager &manager){
        auto avxMemoryTraits = AVX_MemoryTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxMemoryTraits.AVXRegisterSize * unrollFactor;
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
                avxMemoryTraits.loadAVXRegister(a + i, simdDataA);
                avxMemoryTraits.loadAVXRegister(b + i, simdDataB);
                result = avxMemoryTraits.areEqual(simdDataA, simdDataB);
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

        auto avxMemoryTraits = AVX_MemoryTraits<T, unrollFactor>();
        auto avxMathTraits = AVX_MathTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxMemoryTraits.AVXRegisterSize * unrollFactor;
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
        avxMemoryTraits.setValue(&simdScale1, scale1);
        avxMemoryTraits.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex) {
            avxRegisterType simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxRegisterType*, dataType*) = temporalStore ?
                    avxMemoryTraits.storeAVXRegisterTemporal : avxMemoryTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxMemoryTraits.loadAVXRegister(data1 + i, simdData1);
                avxMemoryTraits.loadAVXRegister(data2 + i, simdData2);
                avxMemoryTraits.loadAVXRegister(result + i, simdResult);
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

        auto avxMemoryTraits = AVX_MemoryTraits<T, unrollFactor>();
        auto avxMathTraits = AVX_MathTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxMemoryTraits.AVXRegisterSize * unrollFactor;
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
        avxMemoryTraits.setValue(&simdScale1, scale1);
        avxMemoryTraits.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex) {
            avxRegisterType simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxRegisterType*, dataType*) = temporalStore ?
                                                                             avxMemoryTraits.storeAVXRegisterTemporal : avxMemoryTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxMemoryTraits.loadAVXRegister(data1 + i, simdData1);
                avxMemoryTraits.loadAVXRegister(data2 + i, simdData2);
                avxMemoryTraits.loadAVXRegister(result + i, simdResult);
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

        auto avxMemoryTraits = AVX_MemoryTraits<T, unrollFactor>();
        auto avxMathTraits = AVX_MathTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxMemoryTraits.AVXRegisterSize * unrollFactor;
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
        avxMemoryTraits.setValue(&simdScale1, scale1);
        avxMemoryTraits.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex) {
            avxRegisterType simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxRegisterType*, dataType*) = temporalStore ?
                                                                             avxMemoryTraits.storeAVXRegisterTemporal : avxMemoryTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxMemoryTraits.loadAVXRegister(data1 + i, simdData1);
                avxMemoryTraits.loadAVXRegister(data2 + i, simdData2);
                avxMemoryTraits.loadAVXRegister(result + i, simdResult);
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

    constexpr inline static T sum(T *data1, unsigned size, unsigned cores, CPU_Manager &manager, bool temporalStore = false) {

        auto avxMemoryTraits = AVX_MemoryTraits<T, unrollFactor>();
        auto avxMathTraits = AVX_MathTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxMemoryTraits.AVXRegisterSize * unrollFactor;
        auto totalBlocks = (size + blockSize - 1) / blockSize;
        auto threadBlockSize = (totalBlocks + cores - 1) / cores;
        auto startBlock = 0, endBlock = 0;
        for (size_t i = 0; i < cores; i++) {
            startBlock = i * threadBlockSize;
            endBlock = std::min(startBlock + threadBlockSize, totalBlocks);
            threadLimits[i].first = startBlock * blockSize;
            threadLimits[i].second = std::min(endBlock * blockSize, size);
        }

        auto sumJob = [&](unsigned startIndex, unsigned endIndex, T &result) {
            avxRegisterType simdData1[unrollFactor];
            void (*storeResultRegister)(const avxRegisterType*, dataType*) = temporalStore ?
                                                                             avxMemoryTraits.storeAVXRegisterTemporal : avxMemoryTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxMemoryTraits.loadAVXRegister(data1 + i, simdData1);
                avxMathTraits.sum(simdData1, &result);
            }
            for (size_t i = limit; i < endIndex; i++) {
                result += data1[i];
            }
        };

        auto result = Thread_Operations::executeJobWithReduction<T>(sumJob, size, cores, threadLimits, manager);
        delete[] threadLimits;
        return result;
    }

    constexpr inline static T dotProduct(T *data1, T *data2, unsigned size, unsigned cores, CPU_Manager &manager, bool temporalStore = false) {

        auto avxMemoryTraits = AVX_MemoryTraits<T, unrollFactor>();
        auto avxMathTraits = AVX_MathTraits<T, unrollFactor>();
        using dataType = typename AVX_MemoryTraits<T, unrollFactor>::DataType;
        using avxRegisterType = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;

        auto threadLimits = new std::pair<unsigned, unsigned>[cores];
        auto blockSize = avxMemoryTraits.AVXRegisterSize * unrollFactor;
        auto totalBlocks = (size + blockSize - 1) / blockSize;
        auto threadBlockSize = (totalBlocks + cores - 1) / cores;
        auto startBlock = 0, endBlock = 0;
        for (size_t i = 0; i < cores; i++) {
            startBlock = i * threadBlockSize;
            endBlock = std::min(startBlock + threadBlockSize, totalBlocks);
            threadLimits[i].first = startBlock * blockSize;
            threadLimits[i].second = std::min(endBlock * blockSize, size);
        }

        auto dotProductJob = [&](unsigned startIndex, unsigned endIndex, T &result) {
            avxRegisterType simdData1[unrollFactor];
            avxRegisterType simdData2[unrollFactor];
            void (*storeResultRegister)(const avxRegisterType*, dataType*) = temporalStore ?
                                                                             avxMemoryTraits.storeAVXRegisterTemporal : avxMemoryTraits.storeAVXRegisterNonTemporal;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                avxMemoryTraits.loadAVXRegister(data1 + i, simdData1);
                avxMathTraits.dotProduct(simdData1, simdData1, &result);
            }
            for (size_t i = limit; i < endIndex; i++) {
                result += *(data1 + i) * *(data2 + i);
            }
        };

        auto result = Thread_Operations::executeJobWithReduction<T>(dotProductJob, size, cores, threadLimits, manager);
        delete[] threadLimits;
        return result;
    }
    
};// STLKR_LinearAlgebra
#endif //STALKER_AVX_OPERATIONS_H
 