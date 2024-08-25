//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_SIMD_OPERATIONS_H
#define STALKER_SIMD_OPERATIONS_H
#include "MathTraits.h"
#include "../../Threading/ThreadOperations.h"

template <typename T, unsigned unrollFactor>
class SIMD_Operations {
    using T_data = typename MemoryTraits<T, unrollFactor>::T_data;
    using T_simd = typename MemoryTraits<T, unrollFactor>::T_simd;
public:

    constexpr inline static void copy(T_data *destination, const T_data  *source, unsigned size, CPU_Manager &manager) {
        auto memory = MemoryTraits<T_data, unrollFactor>();
        auto copyThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L1CacheSize) {
            T_simd simdData[unrollFactor];
            void (*storeResultRegister)(const T_simd*, T_data*) = manager.getTemporalCacheStore() ? memory.temporalStore : memory.nonTemporalStore;
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(source + i, simdData);
                storeResultRegister(simdData, destination + i);
            }
            for (size_t i = limit; i < endIndex; i++)
                destination[i] = source[i];
        };
        ThreadOperations::executeJob(copyThreadJob, size, memory.blockSize, manager);
    }

    constexpr inline static void setValue(T_data *destination, T_data  value, unsigned size, CPU_Manager &manager) {
        auto memory = MemoryTraits<T_data, unrollFactor>();
        auto setValueThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            T_simd simdData[unrollFactor];
            void (*storeResultRegister)(const T_simd*, T_data*) = manager.getTemporalCacheStore() ? memory.temporalStore : memory.nonTemporalStore;
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.setValue(simdData, value);
                storeResultRegister(simdData, destination + i);
            }
            for (size_t i = limit; i < endIndex; i++)
                destination[i] = value;
        };

        ThreadOperations::executeJob(setValueThreadJob, size, memory.blockSize, manager);
    }

    constexpr inline static void scale(T_data *data, T_data  scale, unsigned size, CPU_Manager &manager) {
        auto memory = MemoryTraits<T_data, unrollFactor>();
        auto math = MathTraits<T_data, unrollFactor>();
        void (*storeResultRegister)(const T_simd*, T_data*) = manager.getTemporalCacheStore() ? memory.temporalStore : memory.nonTemporalStore;
        auto scaleThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            T_simd simdScale;
            memory.setValue(&simdScale, scale);
            T_simd simdData[unrollFactor];
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(data + i, simdData);
                math.scale(simdData, &simdScale);
                storeResultRegister(simdData, data + i);
            }
            for (size_t i = limit; i < endIndex; i++)
                data[i] *= scale;
        };
        ThreadOperations::executeJob(scaleThreadJob, size, memory.blockSize, manager);
    }

    constexpr inline static bool areEqual(const T_data *a, const T_data *b, unsigned size, CPU_Manager &manager){
        auto memory = MemoryTraits<T_data, unrollFactor>();
        bool result = true;
        auto areEqualThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            T_simd simdDataA[unrollFactor], simdDataB[unrollFactor];
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(a + i, simdDataA);
                memory.loadRegister(b + i, simdDataB);
                result = memory.areEqual(simdDataA, simdDataB);
                if (!result)
                    return;
            }
            for (size_t i = limit; i < endIndex; i++) {
                if (a[i] != b[i]) {
                    result = false;
                    return;
                }
            }
        };
        ThreadOperations::executeJob(areEqualThreadJob, size, memory.blockSize, manager);
        return result;
    }

    constexpr inline static void add(T_data *data1, T_data  *data2, T_data *result, T_data scale1, T_data scale2, unsigned size, CPU_Manager &manager) {

        auto memory = MemoryTraits<T_data, unrollFactor>();
        auto math = MathTraits<T_data, unrollFactor>();
        void (*storeResultRegister)(const T_simd*, T_data*) = manager.getTemporalCacheStore() ? memory.temporalStore : memory.nonTemporalStore;
        
        auto addJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            T_simd simdScale1, simdScale2;
            memory.setValue(&simdScale1, scale1);
            memory.setValue(&simdScale2, scale2);
            T_simd simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(data1 + i, simdData1);
                memory.loadRegister(data2 + i, simdData2);
                memory.loadRegister(result + i, simdResult);
                math.add(simdData1, simdData2, simdResult, &simdScale1, &simdScale2);
                storeResultRegister(simdResult, result + i);
            }
            for (size_t i = limit; i < endIndex; i++)
                result[i] = data1[i] * scale1 + data2[i] * scale2;
        };
        ThreadOperations::executeJob(addJob, size, memory.blockSize, manager);
    }

    constexpr inline static void subtract(T_data *data1, T_data  *data2, T_data *result, T_data scale1, T_data scale2, unsigned size, CPU_Manager &manager) {
        auto memory = MemoryTraits<T_data, unrollFactor>();
        auto math = MathTraits<T_data, unrollFactor>();
        void (*storeResultRegister)(const T_simd*, T_data*) = manager.getTemporalCacheStore() ? memory.temporalStore : memory.nonTemporalStore;
        auto subtractJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            T_simd simdScale1, simdScale2;
            memory.setValue(&simdScale1, scale1);
            memory.setValue(&simdScale2, scale2);
            T_simd simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(data1 + i, simdData1);
                memory.loadRegister(data2 + i, simdData2);
                memory.loadRegister(result + i, simdResult);
                math.subtract(simdData1, simdData2, simdResult, &simdScale1, &simdScale2);
                storeResultRegister(simdResult, result + i);
            }
            for (size_t i = limit; i < endIndex; i++) 
                result[i] = data1[i] * scale1 - data2[i] * scale2;
        };
        ThreadOperations::executeJob(subtractJob, size, memory.blockSize, manager);
    }

    constexpr inline static void multiply(T_data *data1, T_data  *data2, T_data *result, T scale1, T_data scale2, unsigned size, CPU_Manager &manager) {
        auto memory = MemoryTraits<T_data, unrollFactor>();
        auto math = MathTraits<T_data, unrollFactor>();
        void (*storeResultRegister)(const T_simd*, T_data*) = manager.getTemporalCacheStore() ? memory.temporalStore : memory.nonTemporalStore;
        auto multiplyJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            T_simd simdScale1, simdScale2;
            memory.setValue(&simdScale1, scale1);
            memory.setValue(&simdScale2, scale2);
            T_simd simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(data1 + i, simdData1);
                memory.loadRegister(data2 + i, simdData2);
                memory.loadRegister(result + i, simdResult);
                math.multiply(simdData1, simdData2, simdResult, &simdScale1, &simdScale2);
                storeResultRegister(simdResult, result + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                result[i] = data1[i] * scale1 * data2[i] * scale2;
            }
        };
        ThreadOperations::executeJob(multiplyJob, size, memory.blockSize, manager);
    }

    constexpr inline static T_data sum(T_data *data1, unsigned size, CPU_Manager &manager) {
        auto memory = MemoryTraits<T_data, unrollFactor>();
        auto math = MathTraits<T_data, unrollFactor>();
        
        auto sumJob = [&](unsigned startIndex, unsigned endIndex, T_data *result, unsigned L0CacheSize) {
            T_simd simdData1[unrollFactor];
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(data1 + i, simdData1);
                math.sum(simdData1, result);
            }
            for (size_t i = limit; i < endIndex; i++)
                *result += data1[i];
        };
        auto result = ThreadOperations::executeJobWithReduction<T_data>(sumJob, size, memory.blockSize, manager);
        return result;
    }

    constexpr inline static T_data dotProduct(T_data *data1, T_data *data2, unsigned size, CPU_Manager &manager) {
        auto memory = MemoryTraits<T_data, unrollFactor>();
        auto math = MathTraits<T_data, unrollFactor>();

        auto dotProductJob = [&](unsigned startIndex, unsigned endIndex, T_data *result, unsigned L0CacheSize) {
            T_simd simdData1[unrollFactor];
            T_simd simdData2[unrollFactor];
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(data1 + i, simdData1);
                math.dotProduct(simdData1, simdData1, result);
            }
            for (size_t i = limit; i < endIndex; i++)
                *result += *(data1 + i) * *(data2 + i);
        };
        auto result = ThreadOperations::executeJobWithReduction<T_data>(dotProductJob, size, memory.blockSize, manager);
        return result;
    }

private:



};// STLKR_LinearAlgebra
#endif //STALKER_SIMD_OPERATIONS_H
 