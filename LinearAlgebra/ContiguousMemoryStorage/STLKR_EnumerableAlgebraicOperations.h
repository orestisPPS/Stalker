//
// Created by hal9000 on 4/17/24.
//

#ifndef STALKER_STLKR_ENUMERABLEALGEBRAICOPERATIONS_H
#define STALKER_STLKR_ENUMERABLEALGEBRAICOPERATIONS_H
#include "STLKR_EnumerableOperationsBase.h"
#include <cmath>
namespace STLKR_LinearAlgebra {
    
    template <unsigned size, typename T, unsigned availableThreads = 1>
    class STLKR_EnumerableAlgebraicOperations : public STLKR_EnumerableOperationsBase<size, T, availableThreads> {
    
    public:
        
        explicit STLKR_EnumerableAlgebraicOperations(T* data) : STLKR_EnumerableOperationsBase<size, T, availableThreads>(data) {}
        
        static constexpr T sum (const T* data){
            auto sumJob = [data](unsigned start, unsigned end) {
                T sum = T(0);
                for (unsigned i = start; i < end; i++) {
                    sum += data[i];
                }
                return sum;
            };
            return STLKR_ThreadingOperations<availableThreads>::executeParallelJobWithReduction(sumJob, size);
        }
        
        static constexpr T add(const T* data1, const T* data2, T* result) {
            auto addJob = [data1, data2, result](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    result[i] = data1[i] + data2[i];
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(addJob, size);

        }
        
        static constexpr T addIntoThis(const T* data1, const T* data2) {
            auto addJob = [data1, data2](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    data1[i] += data2[i];
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(addJob, size);
        }
        static constexpr T subtract(const T* data1, const T* data2, T* result) {
            auto subtractJob = [data1, data2, result](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    result[i] = data1[i] - data2[i];
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(subtractJob, size);
        }
        
        static constexpr T subtractIntoThis(const T* data1, const T* data2) {
            auto subtractJob = [data1, data2](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    data1[i] -= data2[i];
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(subtractJob, size);
        }
        
        static constexpr void scale(T* data, T scalar) {
            auto scaleJob = [data, scalar](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    data[i] *= scalar;
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(scaleJob, size);
        }
        
        static constexpr T dot(const T* data1, const T* data2) {
            auto dotJob = [data1, data2](unsigned start, unsigned end) {
                T sum = T(0);
                for (unsigned i = start; i < end; i++) {
                    sum += data1[i] * data2[i];
                }
                return sum;
            };
            return STLKR_ThreadingOperations<availableThreads>::executeParallelJobWithReduction(dotJob, size);
        }
        
        double magnitude(const T* data) {
            auto magnitudeJob = [data](unsigned start, unsigned end) {
                T sum = T(0);
                for (unsigned i = start; i < end; i++) {
                    sum += data[i] * data[i];
                }
            };
            return std::sqrt(STLKR_ThreadingOperations<availableThreads>::executeParallelJobWithReduction(magnitudeJob, size));
        }
        
        void normalize(T* data, double customMagnitude = 0) {
            double magnitude = customMagnitude == 0 ? magnitude(data) : customMagnitude;
            auto normalizeJob = [data, magnitude](unsigned start, unsigned end) {
                for (unsigned i = start; i < end; i++) {
                    data[i] /= magnitude;
                }
            };
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(normalizeJob, size);
        }
        
        double distance(const T* data1, const T* data2) {
            auto distanceJob = [data1, data2](unsigned start, unsigned end) {
                T sum = T(0);
                for (unsigned i = start; i < end; i++) {
                    sum += (data1[i] - data2[i]) * (data1[i] - data2[i]);
                }
                return sum;
            };
            return std::sqrt(STLKR_ThreadingOperations<availableThreads>::executeParallelJobWithReduction(distanceJob, size));
        }
        
        double angle(const T* data1, const T* data2) {
            return std::acos(dot(data1, data2) / (magnitude(data1) * magnitude(data2)));
        }
        


#endif //STALKER_STLKR_ENUMERABLEALGEBRAICOPERATIONS_H
