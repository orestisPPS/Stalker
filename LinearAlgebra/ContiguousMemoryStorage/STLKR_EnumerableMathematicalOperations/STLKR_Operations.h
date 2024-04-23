//
// Created by hal9000 on 4/17/24.
//

#ifndef STALKER_STLKR_OPERATIONS_H
#define STALKER_STLKR_OPERATIONS_H
#include "../../../ThreadingOperations/STLKR_ThreadingOperations.h"

namespace STLKR_LinearAlgebra {
    template <typename T, size_t size, size_t availableThreads = 1>
    class STLKR_Operations {
        
    public:
        
        static inline constexpr void setConstantValue (T* data, T value){
            auto setConstantValueJob = [&](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    data[i] = value;
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(setConstantValueJob, size);
            return data;
        }
        static inline constexpr void deepCopy(const T* source, T* destination){
            auto deepCopyJob = [&](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    destination[i] = source[i];
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(deepCopyJob, size);
        }
        static inline constexpr bool areEqual(const T* data1, const T* data2){
            auto areEqualJob = [&](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    if (data1[i] != data2[i]) {
                        return false;
                    }
                }
                return true;
            };
            return STLKR_ThreadingOperations<availableThreads>::executeParallelJobWithReduction(areEqualJob, size);
        }

        static inline constexpr T sum(const T* data){
            auto sumJob = [&](unsigned start, unsigned end) {
                T sum = T(0);
                for (unsigned i = start; i < end; i++) {
                    sum += data[i];
                }
                return sum;
            };
            return STLKR_ThreadingOperations<availableThreads>::executeParallelJobWithReduction(sumJob, size);
        }
        
        static inline constexpr void add(const T* data1, T scale1, const T* data2, T scale2, T* result){
            auto addJob = [&](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    result[i] = data1[i] * scale1 + data2[i] * scale2;
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(addJob, size);
        }

        static inline constexpr void addIntoThis(const T* data1, T scale1, const T* data2, T scale2){
            auto addJob = [&](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    data1[i] = data1[i] * scale1 + data2[i] * scale2;
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(addJob, size);
        }

        static inline constexpr void subtract(const T* data1, T scale1, const T* data2, T scale2, T* result){
            auto subtractJob = [&](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    result[i] = data1[i] * scale1 - data2[i] * scale2;
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(subtractJob, size);
        }

        static inline constexpr void subtractIntoThis(const T* data1, T scale1, const T* data2, T scale2){
            auto subtractJob = [&](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    data1[i] = data1[i] * scale1 - data2[i] * scale2;
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(subtractJob, size);
        }

        static inline constexpr T dotProduct(const T* data1, const T* data2){
            auto dotJob = [&](unsigned start, unsigned end) {
                T sum = T(0);
                for (unsigned i = start; i < end; i++) {
                    sum += data1[i] * data2[i];
                }
                return sum;
            };
            return STLKR_ThreadingOperations<availableThreads>::executeParallelJobWithReduction(dotJob, size);
        }

        static inline constexpr void divide(const T* data1, T scale1, const T* data2, T scale2, T* result) {
            auto divideJob = [&](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    result[i] = data1[i] * scale1 / (data2[i] * scale2);
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(divideJob, size);
        }
    };
}
#endif //STALKER_STLKR_OPERATIONS_H
