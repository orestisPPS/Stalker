//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_STLKR_OPERATIONS_SIMD_H
#define STALKER_STLKR_OPERATIONS_SIMD_H
#include <immintrin.h>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>
#include "../STLKR_Enumerable/STLKR_SIMD/STLKR_SIMD.h"
#include "../../../ThreadingOperations/STLKR_ThreadingOperations.h"
using namespace STLKR_LinearAlgebra;

enum class STLKR_SIMD_Prefetch_Hint {
    T0 = _MM_HINT_T0,
    T1 = _MM_HINT_T1,
    T2 = _MM_HINT_T2,
    NTA = _MM_HINT_NTA
};

enum class STLKR_SIMD_Prefetch_Distance {
    _16 = 16,
    _32 = 32,
    _64 = 64,
    _128 = 128,
    _256 = 256
};

enum class STLKR_SIMD_Stores{
    Regular = 0,
    NonTemporal = 1
};

struct STLKR_SIMD_Prefetch_Config {
    STLKR_SIMD_Prefetch_Hint hint = STLKR_SIMD_Prefetch_Hint::T0;
    STLKR_SIMD_Prefetch_Distance distance = STLKR_SIMD_Prefetch_Distance::_64;
    STLKR_SIMD_Stores storeType = STLKR_SIMD_Stores::Regular;
};

template <typename T, size_t size, size_t availableThreads = 1>
class STLKR_Operations_SIMD {

public:

    constexpr inline static void deepcopy(T *destination, T *source) {
        auto registerSize = STLKR_SIMD_DataTypeSize.at(type_index(typeid(T)));
        auto limit = size - (size % AVX_DOUBLE_SIZE);
        
        if constexpr (is_same_v<T, double>) {
            for (unsigned i = 0; i < limit; i += AVX_DOUBLE_SIZE * 4) {
                __m256d va1 = _mm256_load_pd(source + i);
                __m256d va2 = _mm256_load_pd(source + i + AVX_DOUBLE_SIZE);
                __m256d va3 = _mm256_load_pd(source + i + 2 * AVX_DOUBLE_SIZE);
                __m256d va4 = _mm256_load_pd(source + i + 3 * AVX_DOUBLE_SIZE);
                _mm256_store_pd(destination + i, va1);
                _mm256_store_pd(destination + i + AVX_DOUBLE_SIZE, va2);
                _mm256_store_pd(destination + i + 2 * AVX_DOUBLE_SIZE, va3);
                _mm256_store_pd(destination + i + 3 * AVX_DOUBLE_SIZE, va4);
            }
        }
        else if constexpr (is_same_v<T, double>) {
            for (unsigned i = 0; i < size - (size % AVX_DOUBLE_SIZE); i += AVX_DOUBLE_SIZE) {
                __m256d va = _mm256_load_pd(source + i);
                _mm256_store_pd(destination + i, va);
            }
        } else if constexpr (is_same_v<T, int> || is_same_v<T, unsigned> || is_same_v<T, short>) {
            for (unsigned i = 0; i < size - (size % AVX_DOUBLE_SIZE); i += AVX_DOUBLE_SIZE) {
                __m256i va = _mm256_loadu_si256((__m256i *)(source + i));
                _mm256_storeu_si256((__m256i *)(destination + i), va);
            }
        } else {
            throw runtime_error("T must be float, double, int, unsigned, or short");
        }
    }

    
    constexpr static void add(double *data1, double scale1, double *data2, double scale2, double *result) {
        size_t limit = size - (size % (AVX_DOUBLE_SIZE * 4));  // Adjusted for unrolling factor
        __m256d va1, va2, va3, va4, ve1, ve2, ve3, ve4, vb1, vb2, vb3, vb4;

        __m256d vScale1 = _mm256_set1_pd(scale1);
        __m256d vScale2 = _mm256_set1_pd(scale2);
        
        if constexpr (std::is_same_v<double, T>) {
            for (size_t i = 0; i < limit; i += 16) {

                va1 = _mm256_load_pd(data1 + i);
                va2 = _mm256_load_pd(data1 + i + 4);
                va3 = _mm256_load_pd(data1 + i + 8);
                va4 = _mm256_load_pd(data1 + i + 12);
                vb1 = _mm256_load_pd(data2 + i);
                vb2 = _mm256_load_pd(data2 + i + 4);
                vb3 = _mm256_load_pd(data2 + i + 8);
                vb4 = _mm256_load_pd(data2 + i + 12);

                ve1 = _mm256_fmadd_pd(va1, vScale1, _mm256_mul_pd(vb1, vScale2));
                ve2 = _mm256_fmadd_pd(va2, vScale1, _mm256_mul_pd(vb2, vScale2));
                ve3 = _mm256_fmadd_pd(va3, vScale1, _mm256_mul_pd(vb3, vScale2));
                ve4 = _mm256_fmadd_pd(va4, vScale1, _mm256_mul_pd(vb4, vScale2));

                _mm256_store_pd(result + i, ve1);
                _mm256_store_pd(result + i + 4, ve2);
                _mm256_store_pd(result + i + 8, ve3);
                _mm256_store_pd(result + i + 12, ve4);
            }
        }
        
        for (size_t i = limit; i < size; i++) {
            result[i] = data1[i] * scale1 + data2[i] * scale2;
        }
    }

    constexpr static void add(double *data1, double scale1, double *data2, double scale2, double *result, STLKR_SIMD_Prefetch_Config prefetchConfig) {
        size_t limit = size - (size % (AVX_DOUBLE_SIZE * 4));  // Adjusted for unrolling factor
        __m256d va1, va2, va3, va4, ve1, ve2, ve3, ve4, vb1, vb2, vb3, vb4;

        __m256d vScale1 = _mm256_set1_pd(scale1);
        __m256d vScale2 = _mm256_set1_pd(scale2);
        int prefetchDistance = static_cast<int>(prefetchConfig.distance);
        auto hint = static_cast<_mm_hint>(prefetchConfig.hint);
        auto storeType = prefetchConfig.storeType;

        if constexpr (std::is_same_v<double, T>) {
            for (size_t i = 0; i < limit; i += 16) {  // Adjust loop increment based on unrolling factor
                // Prefetch data that will be needed soon
                if (i + prefetchDistance < size) {
                    _mm_prefetch((const char*)(data1 + i + prefetchDistance), hint);
                    _mm_prefetch((const char*)(data2 + i + prefetchDistance), hint);
                    _mm_prefetch((const char*)(result + i + prefetchDistance), hint);
                }

                va1 = _mm256_load_pd(data1 + i);
                va2 = _mm256_load_pd(data1 + i + 4);
                va3 = _mm256_load_pd(data1 + i + 8);
                va4 = _mm256_load_pd(data1 + i + 12);
                vb1 = _mm256_load_pd(data2 + i);
                vb2 = _mm256_load_pd(data2 + i + 4);
                vb3 = _mm256_load_pd(data2 + i + 8);
                vb4 = _mm256_load_pd(data2 + i + 12);

                ve1 = _mm256_fmadd_pd(va1, vScale1, _mm256_mul_pd(vb1, vScale2));
                ve2 = _mm256_fmadd_pd(va2, vScale1, _mm256_mul_pd(vb2, vScale2));
                ve3 = _mm256_fmadd_pd(va3, vScale1, _mm256_mul_pd(vb3, vScale2));
                ve4 = _mm256_fmadd_pd(va4, vScale1, _mm256_mul_pd(vb4, vScale2));

                switch (storeType) {
                    case STLKR_SIMD_Stores::Regular:
                        _mm256_store_pd(result + i, ve1);
                        _mm256_store_pd(result + i + 4, ve2);
                        _mm256_store_pd(result + i + 8, ve3);
                        _mm256_store_pd(result + i + 12, ve4);
                        break;
                    case STLKR_SIMD_Stores::NonTemporal:
                        _mm256_stream_pd(result + i, ve1);
                        _mm256_stream_pd(result + i + 4, ve2);
                        _mm256_stream_pd(result + i + 8, ve3);
                        _mm256_stream_pd(result + i + 12, ve4);
                        break;
                    default:
                        throw runtime_error("Invalid store type");
                }
            }
        }

        for (size_t i = limit; i < size; i++) {
            result[i] = data1[i] * scale1 + data2[i] * scale2;
        }
    }





};// STLKR_LinearAlgebra
#endif //STALKER_STLKR_OPERATIONS_SIMD_H
