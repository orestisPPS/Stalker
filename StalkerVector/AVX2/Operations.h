//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_SIMD_OPERATIONS_H
#define STALKER_SIMD_OPERATIONS_H
#include "MathTraits.h"
#include "../../Threading/ThreadOperations.h"

template <typename T, unsigned unrollFactor>
class SIMD_Operations {
    using avxT = typename AVX_MemoryTraits<T, unrollFactor>::AVXRegisterType;
    using memory = AVX_MemoryTraits<T, unrollFactor>();
    using math = MathTraits<T, unrollFactor>();
public:

    constexpr inline static void copy(T *destination, T  *source, unsigned size, CPU_Manager &manager, bool temporalStore = false) {
        auto memory = AVX_MemoryTraits<T, unrollFactor>();
        auto copyThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L1CacheSize) {
            avxT simdData[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ?
                                                           memory.temporalStore : memory.nonTemporalStore;
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

    constexpr inline static void setValue(T *destination, T  value, unsigned size, CPU_Manager &manager, bool temporalStore = false) {
        auto memory = AVX_MemoryTraits<T, unrollFactor>();
        auto setValueThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdData[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ?
               memory.temporalStore : memory.nonTemporalStore;
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.setValue(simdData, value);
                storeResultRegister(simdData, destination + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                destination[i] = value;
            }
        };

        ThreadOperations::executeJob(setValueThreadJob, size, memory.blockSize, manager);
    }

    constexpr inline static void scale(T *data, T  scale, unsigned size, CPU_Manager &manager, bool temporalStore = false) {
        auto memory = AVX_MemoryTraits<T, unrollFactor>();
        auto math = MathTraits<T, unrollFactor>();
        avxT simdScale;
        memory.setValue(&simdScale, scale);
        auto scaleThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdData[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ?
                                                           memory.temporalStore : memory.nonTemporalStore;
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(data + i, simdData);
                math.scale(simdData, &simdScale);
                storeResultRegister(simdData, data + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                data[i] *= scale;
            }
        };
        ThreadOperations::executeJob(scaleThreadJob, size, memory.blockSize, manager);
    }

    constexpr inline static bool areEqual(const T *a, const T *b, unsigned size, CPU_Manager &manager){
        auto memory = AVX_MemoryTraits<T, unrollFactor>();
        bool result = true;
        auto areEqualThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdDataA[unrollFactor];
            avxT simdDataB[unrollFactor];
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

    constexpr inline static void add(T *data1, T  *data2, T *result, T scale1, T scale2, unsigned size, CPU_Manager &manager, bool temporalStore = false) {

        auto memory = AVX_MemoryTraits<T, unrollFactor>();
        auto math = MathTraits<T, unrollFactor>();
        
        avxT simdScale1, simdScale2;
        memory.setValue(&simdScale1, scale1);
        memory.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ? memory.temporalStore : memory.nonTemporalStore;
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(data1 + i, simdData1);
                memory.loadRegister(data2 + i, simdData2);
                memory.loadRegister(result + i, simdResult);
                math.add(simdData1, simdData2, simdResult, &simdScale1, &simdScale2);
                storeResultRegister(simdResult, result + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                result[i] = data1[i] * scale1 + data2[i] * scale2;
            }
        };
        ThreadOperations::executeJob(addJob, size, memory.blockSize, manager);
    }

    constexpr inline static void subtract(T *data1, T  *data2, T *result, T scale1, T scale2, unsigned size, CPU_Manager &manager, bool temporalStore = false) {

        auto memory = AVX_MemoryTraits<T, unrollFactor>();
        auto math = MathTraits<T, unrollFactor>();
        
        avxT simdScale1, simdScale2;
        memory.setValue(&simdScale1, scale1);
        memory.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ? memory.temporalStore : memory.nonTemporalStore;
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(data1 + i, simdData1);
                memory.loadRegister(data2 + i, simdData2);
                memory.loadRegister(result + i, simdResult);
                math.subtract(simdData1, simdData2, simdResult, &simdScale1, &simdScale2);
                storeResultRegister(simdResult, result + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                result[i] = data1[i] * scale1 - data2[i] * scale2;
            }
        };
        ThreadOperations::executeJob(addJob, size, memory.blockSize, manager);
    }

    constexpr inline static void multiply(T *data1, T  *data2, T *result, T scale1, T scale2, unsigned size, CPU_Manager &manager, bool temporalStore = false) {
        auto memory = AVX_MemoryTraits<T, unrollFactor>();
        auto math = MathTraits<T, unrollFactor>();

        avxT simdScale1, simdScale2;
        memory.setValue(&simdScale1, scale1);
        memory.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ?
                                                           memory.temporalStore : memory.nonTemporalStore;
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
        ThreadOperations::executeJob(addJob, size, memory.blockSize, manager);
    }

    constexpr inline static T sum(T *data1, unsigned size, CPU_Manager &manager) {
        auto memory = AVX_MemoryTraits<T, unrollFactor>();
        auto math = MathTraits<T, unrollFactor>();
        
        auto sumJob = [&](unsigned startIndex, unsigned endIndex, T *result, unsigned L0CacheSize) {
            avxT simdData1[unrollFactor];
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(data1 + i, simdData1);
                math.sum(simdData1, result);
            }
            for (size_t i = limit; i < endIndex; i++)
                *result += data1[i];
        };
        auto result = ThreadOperations::executeJobWithReduction<T>(sumJob, size, memory.blockSize, manager);
        return result;
    }

    constexpr inline static T dotProduct(T *data1, T *data2, unsigned size, CPU_Manager &manager) {
        auto memory = AVX_MemoryTraits<T, unrollFactor>();
        auto math = MathTraits<T, unrollFactor>();

        auto dotProductJob = [&](unsigned startIndex, unsigned endIndex, T *result, unsigned L0CacheSize) {
            avxT simdData1[unrollFactor];
            avxT simdData2[unrollFactor];
            auto limit = endIndex - (endIndex % memory.blockSize);
            for (size_t i = startIndex; i < limit; i += memory.blockSize) {
                memory.loadRegister(data1 + i, simdData1);
                math.dotProduct(simdData1, simdData1, result);
            }
            for (size_t i = limit; i < endIndex; i++)
                *result += *(data1 + i) * *(data2 + i);
        };
        auto result = ThreadOperations::executeJobWithReduction<T>(dotProductJob, size, memory.blockSize, manager);
        return result;
    }

private:



};// STLKR_LinearAlgebra
#endif //STALKER_SIMD_OPERATIONS_H
 