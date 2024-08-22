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
public:

    constexpr inline static void copy(T *destination, T  *source, unsigned size, CPU_Manager &manager, bool temporalStore = false) {

        auto simdMemory = AVX_MemoryTraits<T, unrollFactor>();
        auto blockSize = simdMemory.registerSize * unrollFactor;

        auto copyThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L1CacheSize) {
            avxT simdData[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ?
                                                           simdMemory.temporalStore : simdMemory.nonTemporalStore;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                simdMemory.loadRegister(source + i, simdData);
                storeResultRegister(simdData, destination + i);
            }
            for (size_t i = limit; i < endIndex; i++)
                destination[i] = source[i];
        };
        ThreadOperations::executeJob(copyThreadJob, size, blockSize, manager);

    }

    constexpr inline static void setValue(T *destination, T  value, unsigned size, CPU_Manager &manager, bool temporalStore = false) {
        auto simdMemory = AVX_MemoryTraits<T, unrollFactor>();
        auto blockSize = simdMemory.registerSize * unrollFactor;

        auto setValueThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdData[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ?
               simdMemory.temporalStore : simdMemory.nonTemporalStore;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                simdMemory.setValue(simdData, value);
                storeResultRegister(simdData, destination + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                destination[i] = value;
            }
        };

        ThreadOperations::executeJob(setValueThreadJob, size, blockSize, manager);
    }

    constexpr inline static void scale(T *data, T  scale, unsigned size, CPU_Manager &manager, bool temporalStore = false) {
        auto simdMemory = AVX_MemoryTraits<T, unrollFactor>();
        auto simdMath = SIMD_MathTraits<T, unrollFactor>();
        auto blockSize = simdMemory.registerSize * unrollFactor;

        avxT simdScale;
        simdMemory.setValue(&simdScale, scale);
        auto scaleThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdData[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ?
                                                           simdMemory.temporalStore : simdMemory.nonTemporalStore;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                simdMemory.loadRegister(data + i, simdData);
                simdMath.scale(simdData, &simdScale);
                storeResultRegister(simdData, data + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                data[i] *= scale;
            }
        };
        ThreadOperations::executeJob(scaleThreadJob, size, blockSize, manager);
    }

    constexpr inline static bool areEqual(const T *a, const T *b, unsigned size, CPU_Manager &manager){
        auto simdMemory = AVX_MemoryTraits<T, unrollFactor>();
        auto blockSize = simdMemory.registerSize * unrollFactor;

        bool result = true;
        auto areEqualThreadJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdDataA[unrollFactor];
            avxT simdDataB[unrollFactor];
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                simdMemory.loadRegister(a + i, simdDataA);
                simdMemory.loadRegister(b + i, simdDataB);
                result = simdMemory.areEqual(simdDataA, simdDataB);
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
        ThreadOperations::executeJob(areEqualThreadJob, size, blockSize, manager);
        return result;
    }

    constexpr inline static void add(T *data1, T  *data2, T *result, T scale1, T scale2, unsigned size, CPU_Manager &manager, bool temporalStore = false) {

        auto simdMemory = AVX_MemoryTraits<T, unrollFactor>();
        auto simdMath = SIMD_MathTraits<T, unrollFactor>();
        auto blockSize = simdMemory.registerSize * unrollFactor;


        avxT simdScale1, simdScale2;
        simdMemory.setValue(&simdScale1, scale1);
        simdMemory.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ? simdMemory.temporalStore : simdMemory.nonTemporalStore;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                simdMemory.loadRegister(data1 + i, simdData1);
                simdMemory.loadRegister(data2 + i, simdData2);
                simdMemory.loadRegister(result + i, simdResult);
                simdMath.add(simdData1, simdData2, simdResult, &simdScale1, &simdScale2);
                storeResultRegister(simdResult, result + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                result[i] = data1[i] * scale1 + data2[i] * scale2;
            }
        };
        ThreadOperations::executeJob(addJob, size, blockSize, manager);
    }

    constexpr inline static void subtract(T *data1, T  *data2, T *result, T scale1, T scale2, unsigned size, CPU_Manager &manager, bool temporalStore = false) {

        auto simdMemory = AVX_MemoryTraits<T, unrollFactor>();
        auto simdMath = SIMD_MathTraits<T, unrollFactor>();
        auto blockSize = simdMemory.registerSize * unrollFactor;


        avxT simdScale1, simdScale2;
        simdMemory.setValue(&simdScale1, scale1);
        simdMemory.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ? simdMemory.temporalStore : simdMemory.nonTemporalStore;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                simdMemory.loadRegister(data1 + i, simdData1);
                simdMemory.loadRegister(data2 + i, simdData2);
                simdMemory.loadRegister(result + i, simdResult);
                simdMath.subtract(simdData1, simdData2, simdResult, &simdScale1, &simdScale2);
                storeResultRegister(simdResult, result + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                result[i] = data1[i] * scale1 - data2[i] * scale2;
            }
        };
        ThreadOperations::executeJob(addJob, size, blockSize, manager);
    }

    constexpr inline static void multiply(T *data1, T  *data2, T *result, T scale1, T scale2, unsigned size, CPU_Manager &manager, bool temporalStore = false) {
        auto simdMemory = AVX_MemoryTraits<T, unrollFactor>();
        auto simdMath = SIMD_MathTraits<T, unrollFactor>();
        auto blockSize = simdMemory.registerSize * unrollFactor;

        avxT simdScale1, simdScale2;
        simdMemory.setValue(&simdScale1, scale1);
        simdMemory.setValue(&simdScale2, scale2);
        auto addJob = [&](unsigned startIndex, unsigned endIndex, unsigned L0CacheSize) {
            avxT simdData1[unrollFactor], simdData2[unrollFactor], simdResult[unrollFactor];
            void (*storeResultRegister)(const avxT*, T*) = temporalStore ?
                                                           simdMemory.temporalStore : simdMemory.nonTemporalStore;
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                simdMemory.loadRegister(data1 + i, simdData1);
                simdMemory.loadRegister(data2 + i, simdData2);
                simdMemory.loadRegister(result + i, simdResult);
                simdMath.multiply(simdData1, simdData2, simdResult, &simdScale1, &simdScale2);
                storeResultRegister(simdResult, result + i);
            }
            for (size_t i = limit; i < endIndex; i++) {
                result[i] = data1[i] * scale1 * data2[i] * scale2;
            }
        };
        ThreadOperations::executeJob(addJob, size, blockSize, manager);
    }

    constexpr inline static T sum(T *data1, unsigned size, CPU_Manager &manager) {
        auto simdMemory = AVX_MemoryTraits<T, unrollFactor>();
        auto simdMath = SIMD_MathTraits<T, unrollFactor>();
        auto blockSize = simdMemory.registerSize * unrollFactor;

        auto sumJob = [&](unsigned startIndex, unsigned endIndex, T *result, unsigned L0CacheSize) {
            avxT simdData1[unrollFactor];
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                simdMemory.loadRegister(data1 + i, simdData1);
                simdMath.sum(simdData1, result);
            }
            for (size_t i = limit; i < endIndex; i++)
                *result += data1[i];
        };
        auto result = ThreadOperations::executeJobWithReduction<T>(sumJob, size, blockSize, manager);
        return result;
    }

    constexpr inline static T dotProduct(T *data1, T *data2, unsigned size, CPU_Manager &manager) {
        auto simdMemory = AVX_MemoryTraits<T, unrollFactor>();
        auto simdMath = SIMD_MathTraits<T, unrollFactor>();
        auto blockSize = simdMemory.registerSize * unrollFactor;

        auto dotProductJob = [&](unsigned startIndex, unsigned endIndex, T *result, unsigned L0CacheSize) {
            avxT simdData1[unrollFactor];
            avxT simdData2[unrollFactor];
            auto limit = endIndex - (endIndex % blockSize);
            for (size_t i = startIndex; i < limit; i += blockSize) {
                simdMemory.loadRegister(data1 + i, simdData1);
                simdMath.dotProduct(simdData1, simdData1, result);
            }
            for (size_t i = limit; i < endIndex; i++)
                *result += *(data1 + i) * *(data2 + i);
        };
        auto result = ThreadOperations::executeJobWithReduction<T>(dotProductJob, size, blockSize, manager);
        return result;
    }

private:



};// STLKR_LinearAlgebra
#endif //STALKER_SIMD_OPERATIONS_H
 