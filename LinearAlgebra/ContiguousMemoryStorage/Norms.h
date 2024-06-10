//
// Created by hal9000 on 4/17/24.
//

#ifndef STALKER_NORMS_H
#define STALKER_NORMS_H

#include <cmath>
#include "../../STLKR_HighPerformance/STLKR_Threading/STLKR_Thread_Operations.h"
namespace STLKR_LinearAlgebra {
    enum Norm_Type {
        // L1 (Manhattan / Taxicab) norm
        // The sum of the absolute values of the vectors' components.
        // For a vector x with n components, the L1 norm is denoted as ||x||1 and defined as:
        // ||x||1 = |x₁| + |x₂| + ... + |xₙ|
        L1,

        // L2 (Euclidean) norm
        // The square root of the sum of the squares of the vectors' components.
        // For a vector x with n components, the L2 norm is denoted as ||x||2 and defined as:
        // ||x||2 = √(x₁² + x₂² + ... + xₙ²)
        L2,

        // L∞ (Chebyshev) norm
        // The maximum absolute value of the vectors' components.
        // For a vector x with n components, the L∞ norm is denoted as ||x||∞ and defined as:
        // ||x||∞ = max(|x₁|, |x₂|, ..., |xₙ|)
        LInf,


        // Lp norm
        // The pth root of the sum of the pth powers of the vectors' components.    
        // For a vector x with n components, the Lp norm is denoted as ||x||p and defined as:
        // ||x||p = (|x₁|^p + |x₂|^p + ... + |xₙ|^p)^(1/p)
        Lp,

/*        //Frobenius (Euclidean for matrices) norm
        // Defined only for Array class. 
        // The square root of the sum of the squares of the matrices' components.
        // For a matrix A with m rows and n columns, the Frobenius norm is denoted as ||A||F and defined as:
        // ||A||F = √(A₁₁² + A₁₂² + ... + Aₘₙ²)
        Frobenius*/
    };
    template <unsigned size, typename T, unsigned availableThreads = 1>

    class Norms {
        
    public:
        
        explicit Norms(T *data) : _data(data) {}
        
        T L1() {
            auto l1Job = [this](unsigned start, unsigned end) {
                T sum = T(0);
                for (unsigned i = start; i < end; i++) {
                    sum += std::abs(_data[i]);
                }
                return sum;
            };
            return STLKR_Thread_Operations<availableThreads>::executeParallelJobWithReduction(l1Job, size);
        }
        
        T L2() {
            auto l2Job = [this](unsigned start, unsigned end) {
                T sum = T(0);
                for (unsigned i = start; i < end; i++) {
                    sum += _data[i] * _data[i];
                }
                return sum;
            };
            T sum = STLKR_Thread_Operations<availableThreads>::executeParallelJobWithReduction(l2Job, size);
            return std::sqrt(sum);
        }
        
        
        
        T LInf() {
            auto lInfJob = [this](unsigned start, unsigned end) {
                T max = T(0);
                for (unsigned i = start; i < end; i++) {
                    max = std::max(max, std::abs(_data[i]));
                }
                return max;
            };
            return STLKR_Thread_Operations<availableThreads>::executeParallelJobWithReduction(lInfJob, size);
        }
        
        T Lp(T p) {
            auto lpJob = [this, p](unsigned start, unsigned end) {
                T sum = T(0);
                for (unsigned i = start; i < end; i++) {
                    sum += std::pow(std::abs(_data[i]), p);
                }
                return sum;
            };
            T sum = STLKR_Thread_Operations<availableThreads>::executeParallelJobWithReduction(lpJob, size);
            return std::pow(sum, 1.0 / p);
        }
        
    private:
        T* _data;
        
    };

} // STLKR_LinearAlgebra

#endif //STALKER_NORMS_H
