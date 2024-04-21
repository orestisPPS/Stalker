//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_STLKR_SIMD_OPERATIONS_H
#define STALKER_STLKR_SIMD_OPERATIONS_H
#include <immintrin.h>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>
#include "../../STLKR_ArithmeticTypes.h"

#define AVX_DOUBLE_SIZE 4
#define AVX_FLOAT_SIZE 8
#define AVX_LONG_DOUBLE_SIZE 2
#define AVX_INT_SIZE 8
#define AVX_SHORT_SIZE 16
#define AVX_UNSIGNED_SIZE 8

#if defined(_WIN32) || defined(_WIN64)
#define AVX_LONG_SIZE 8
#elif defined(__linux__) || defined(__APPLE__)
#define AVX_LONG_SIZE 4
#define AVX_LONG_LONG_SIZE 4
#else
#define AVX_LONG_SIZE 4
    #define AVX_LONG_LONG_SIZE 4
#endif
#define AVX_CHAR_SIZE 32

namespace STLKR_LinearAlgebra {
    static const std::map<std::type_index, unsigned> STLKR_SIMD_DataTypeSize = {
            {std::type_index(typeid(float)), AVX_FLOAT_SIZE},
            {std::type_index(typeid(double)), AVX_DOUBLE_SIZE},
            {std::type_index(typeid(long double)), AVX_LONG_DOUBLE_SIZE},
            {std::type_index(typeid(int)), AVX_INT_SIZE},
            {std::type_index(typeid(long)), AVX_LONG_SIZE},
            {std::type_index(typeid(unsigned long)), AVX_LONG_SIZE},
            {std::type_index(typeid(long long)), AVX_LONG_LONG_SIZE},
            {std::type_index(typeid(unsigned long long)), AVX_LONG_LONG_SIZE},
            {std::type_index(typeid(short)), AVX_SHORT_SIZE},
            {std::type_index(typeid(unsigned short)), AVX_SHORT_SIZE},
            {std::type_index(typeid(char)), AVX_CHAR_SIZE},
            {std::type_index(typeid(unsigned char)), AVX_CHAR_SIZE},
            {std::type_index(typeid(signed char)), AVX_CHAR_SIZE},
            {std::type_index(typeid(unsigned)), AVX_UNSIGNED_SIZE}
    };
    
    template <typename T>
    class STLKR_SIMD_Operations {
    public:
        static void add(const T *data1, const T *data2, T *result, unsigned size) {
//            static_assert(STLKR_SIMD_DataTypeSize.find(std::type_index(typeid(T))) != STLKR_SIMD_DataTypeSize.end(),
//                          "Unsupported data type for SIMD operations");
            unsigned simdBlockSize = STLKR_SIMD_DataTypeSize.at(std::type_index(typeid(T)));

            // SIMD processing loop
            unsigned i = 0;
            for (; i < size - (size % simdBlockSize); i += simdBlockSize) {
                if constexpr (std::is_same_v<T, float>) {
                    __m256 va = _mm256_loadu_ps(data1 + i);
                    __m256 vb = _mm256_loadu_ps(data2 + i);
                    __m256 vc = _mm256_add_ps(va, vb);
                    _mm256_storeu_ps(result + i, vc);
                } else if constexpr (std::is_same_v<T, double>) {
                    __m256d va = _mm256_loadu_pd(data1 + i);
                    __m256d vb = _mm256_loadu_pd(data2 + i);
                    __m256d vc = _mm256_add_pd(va, vb);
                    _mm256_storeu_pd(result + i, vc);
                    
                } else
                    throw std::runtime_error("Unsupported data type for SIMD operations");
            }

            // Scalar processing for remaining elements
            for (; i < size; i++) {
                result[i] = data1[i] + data2[i];
            }
        }
        


    };
} // STLKR_LinearAlgebra
#endif //STALKER_STLKR_SIMD_OPERATIONS_H
