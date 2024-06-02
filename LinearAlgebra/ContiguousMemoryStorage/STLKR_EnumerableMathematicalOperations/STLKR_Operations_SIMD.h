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
#include <iomanip>
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
    STLKR_SIMD_Prefetch_Distance distance = STLKR_SIMD_Prefetch_Distance::_128;
    STLKR_SIMD_Stores storeType = STLKR_SIMD_Stores::Regular;
};

template <typename T, size_t size, size_t unrollFactor, size_t availableThreads = 1>
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


    constexpr static void add(double *data1, double scale1, double *data2, double scale2, double *result, STLKR_SIMD_Prefetch_Config prefetchConfig) {
        size_t limit = size - (size % (AVX_DOUBLE_SIZE * 4));  // Adjusted for unrolling factor
        __m256d va1, va2, va3, va4, ve1, ve2, ve3, ve4, vb1, vb2, vb3, vb4;

        __m256d vScale1 = _mm256_set1_pd(scale1);
        __m256d vScale2 = _mm256_set1_pd(scale2);
        int prefetchDistance = static_cast<int>(prefetchConfig.distance);
        auto hint = static_cast<_mm_hint>(prefetchConfig.hint);
        auto storeType = prefetchConfig.storeType;


        auto storeFunc = (storeType == STLKR_SIMD_Stores::Regular) ?
                         [](double* dest, __m256d val) { _mm256_store_pd(dest, val); } :
                         [](double* dest, __m256d val) { _mm256_stream_pd(dest, val); };
        
        if constexpr (std::is_same_v<double, T>) {
            for (size_t i = 0; i < limit; i += 4
                    ) {  // Adjust loop increment based on unrolling factor
                // Prefetch data that will be needed soon
                if (i + prefetchDistance < size) {
//                    _mm_prefetch((const char*)(data1 + i + prefetchDistance), hint);
//                    _mm_prefetch((const char*)(data2 + i + prefetchDistance), hint);
//                    _mm_prefetch((const char*)(result + i + prefetchDistance), hint);
                }

                va1 = _mm256_load_pd(data1 + i);
                //va2 = _mm256_load_pd(data1 + i + 4);
                //va3 = _mm256_load_pd(data1 + i + 8);
                //va4 = _mm256_load_pd(data1 + i + 12);
                vb1 = _mm256_load_pd(data2 + i);
                //vb2 = _mm256_load_pd(data2 + i + 4);
                //vb3 = _mm256_load_pd(data2 + i + 8);
                //vb4 = _mm256_load_pd(data2 + i + 12);

                ve1 = _mm256_fmadd_pd(va1, vScale1, _mm256_mul_pd(vb1, vScale2));
                //ve2 = _mm256_fmadd_pd(va2, vScale1, _mm256_mul_pd(vb2, vScale2));
                //ve3 = _mm256_fmadd_pd(va3, vScale1, _mm256_mul_pd(vb3, vScale2));
                //ve4 = _mm256_fmadd_pd(va4, vScale1, _mm256_mul_pd(vb4, vScale2));


                storeFunc(result + i, ve1);
            }
        }

        for (size_t i = limit; i < size; i++) {
            result[i] = data1[i] * scale1 + data2[i] * scale2;
        }
    }


    template<size_t numVectors>
    static constexpr inline void addUnrolled(double** data, double* scaleFactors, double* result, STLKR_SIMD_Prefetch_Config prefetchConfig) {
        __m256d simdData[numVectors * unrollFactor];
        __m256d simdScalars[numVectors];
        __m256d simdResult[unrollFactor];
        _loadScaleRegisters<numVectors>(scaleFactors, simdScalars);
        void (*storeResultRegister)(__m256d*, double*) = (prefetchConfig.storeType == STLKR_SIMD_Stores::Regular) ?
                _temporalStoreDouble<unrollFactor> : _nonTemporalStoreDouble<unrollFactor>;

        size_t limit = size - (size % (AVX_DOUBLE_SIZE * unrollFactor));
        
        for (size_t i = 0; i < limit; i += AVX_DOUBLE_SIZE * unrollFactor) {
            _loadDoubleVectorsRegisters<numVectors>(data, simdData, i);
            _fusedMultiplyAddDoubleVectors<numVectors>(simdData, simdScalars, simdResult);
            storeResultRegister(simdResult, result + i);
            _zeroOutResultRegisters<unrollFactor>(simdResult);
        }
        for (size_t i = limit; i < size; i++) {
            for (size_t j = 0; j < numVectors; j++) {
                result[i] += data[j][i] * scaleFactors[j];
            }
        }
    }

private:

    template<size_t numVector>
    static constexpr inline void _loadScaleRegisters(const double* scaleFactors, __m256d *simdScalars) {
        if constexpr (numVector > 0) {
            cout<<"scale factor = " <<  *(scaleFactors + numVector - 1) << " index = " << numVector - 1 << endl;
            simdScalars[numVector - 1] = _mm256_set1_pd(*(scaleFactors + numVector - 1));
            _loadScaleRegisters<numVector - 1>(scaleFactors, simdScalars);
        }
        else return;
    }
    
    template<size_t numUnroll>
    static constexpr inline void _loadDoubleRegisters(double* data, __m256d* simdData) {
        if constexpr (numUnroll > 0) {
            *(simdData + numUnroll - 1) = _mm256_load_pd(data + (numUnroll - 1) * 4);
            _loadDoubleRegisters<numUnroll - 1>(data, simdData);
        }
        else return;
    }
    
    template<size_t numVector>
    static constexpr  inline void _loadDoubleVectorsRegisters(double** data, __m256d* simdData, const unsigned int &index) {
        if constexpr (numVector > 0) {
            _loadDoubleRegisters<unrollFactor>(*(data + numVector - 1) + index, simdData + (numVector - 1) * unrollFactor);
            _loadDoubleVectorsRegisters<numVector - 1>(data, simdData, index);
        }
        else return;
    }

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
    
    template<size_t iUnroll>
    static constexpr inline void _temporalStoreDouble( __m256d *result, double *destination) {
        if constexpr (iUnroll > 0) {
            _mm256_store_pd(destination + (iUnroll - 1) * AVX_DOUBLE_SIZE, *(result + iUnroll - 1));
            _temporalStoreDouble<iUnroll - 1>(result, destination);
        }
        else return;
    }

    template<size_t iUnroll>
    static constexpr inline void _nonTemporalStoreDouble(__m256d *result, double *destination) {
        if constexpr (iUnroll > 0) {
            _mm256_stream_pd(destination + (iUnroll - 1) * AVX_DOUBLE_SIZE, *(result + iUnroll - 1));
            _nonTemporalStoreDouble<iUnroll - 1>(result, destination);
        }
        else return;
    }

    template<size_t iUnroll>
    static constexpr inline void _zeroOutResultRegisters(__m256d *simdData) {
        if constexpr (iUnroll > 0) {
            *(simdData + iUnroll - 1) = _mm256_setzero_pd();
            _zeroOutResultRegisters<iUnroll - 1>(simdData);
        }
        else return;
    }
    
};// STLKR_LinearAlgebra
#endif //STALKER_STLKR_OPERATIONS_SIMD_H
