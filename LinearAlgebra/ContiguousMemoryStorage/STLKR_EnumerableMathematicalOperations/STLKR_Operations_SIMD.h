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



template <typename T, size_t size, size_t availableThreads = 1>
class STLKR_Operations_SIMD {

public:

    constexpr static void deepcopy(T *destination, T *source, const STLKR_SIMD &simdPolicy) {
        const auto &avxVersion = simdPolicy.getAVXType();
        auto registerSize = STLKR_SIMD_DataTypeSize.at(type_index(typeid(T)));
        if (avxVersion == SIMD_AVXType::AVX || avxVersion == SIMD_AVXType::AVX_2) {
            for (unsigned i = 0; i < size - (size % registerSize); i += registerSize) {
                if constexpr (is_same_v<T, float>) {
                    __m256 va = _mm256_loadu_ps(source + i);
                    _mm256_storeu_ps(destination + i, va);
                }
                else if constexpr (is_same_v<T, double>) {
                    __m256d va = _mm256_loadu_pd(source + i);
                    _mm256_storeu_pd(destination + i, va);
                }
                else if constexpr (is_same_v<T, int> || is_same_v<T, unsigned> || is_same_v<T, short>) {
                    __m256i va = _mm256_loadu_si256((__m256i *)(source + i));
                    _mm256_storeu_si256((__m256i *)(destination + i), va);
                }
                else throw runtime_error("SIMD version not supported");
            }
        } else throw runtime_error("SIMD version not supported");
    }

    constexpr static void deepcopy2(T *destination, const T *source, const STLKR_SIMD &simdPolicy) {
        const auto &avxVersion = simdPolicy.getAVXType();
        if (avxVersion == SIMD_AVXType::AVX || avxVersion == SIMD_AVXType::AVX_2) {
            auto registerSize = STLKR_SIMD_DataTypeSize.at(type_index(typeid(T)));

            auto simdCopyJob = [&](unsigned start, unsigned end) {
                // Ensure the starting and ending points are aligned for SIMD operations.
                start = (start / registerSize) * registerSize;
                end = (end < size ? (end / registerSize) * registerSize : size);

                for (unsigned i = start; i < end; i += registerSize) {
                    if constexpr (is_same_v<T, float>) {
                        __m256 va = _mm256_loadu_ps(source + i);
                        _mm256_storeu_ps(destination + i, va);
                    } else if constexpr (is_same_v<T, double>) {
                        __m256d va = _mm256_loadu_pd(source + i);
                        _mm256_storeu_pd(destination + i, va);
                    } else if constexpr (is_same_v<T, int> || is_same_v<T, unsigned> || is_same_v<T, short>) {
                        __m256i va = _mm256_loadu_si256((__m256i *)(source + i));
                        _mm256_storeu_si256((__m256i *)(destination + i), va);
                    } else {
                        throw runtime_error("SIMD version not supported for this type");
                    }
                }
            };

            // Dispatch the job into the pre-existing multithreaded execution framework
            STLKR_ThreadingOperations<availableThreads>::executeParallelJob(simdCopyJob, size);
        } else {
            throw runtime_error("Unsupported SIMD version");
        }
    }

//    static inline constexpr bool areEqual(const T* data1, const T* data2, const STLKR_SIMD &simdPolicy) {
//        const auto &avxVersion = simdPolicy.getAVXType();
//        if (avxVersion == SIMD_AVXType::AVX || avxVersion == SIMD_AVXType::AVX_2) {
//            auto registerSize = STLKR_SIMD_DataTypeSize.at(type_index(typeid(T)));
//            auto blockSize = registerSize * (size / registerSize); // Calculate full blocks that fit into the size
//
//            auto simdCompareJob = [&](unsigned start, unsigned end) {
//                // Ensure the starting and ending points are aligned for SIMD operations.
//                start = (start / registerSize) * registerSize;
//                end = (end < size ? (end / registerSize) * registerSize : size);
//
//                for (unsigned i = start; i < end; i += registerSize) {
//                    if constexpr (is_same_v<T, float>) {
//                        __m256 va = _mm256_loadu_ps(data1 + i);
//                        __m256 vb = _mm256_loadu_ps(data2 + i);
//                        __m256i vc = _mm256_castps_si256(_mm256_cmp_ps(va, vb, _CMP_EQ_OQ));
//                        if (_mm256_movemask_epi8(vc) != 0xFFFFFFFF) {
//                            return false;
//                        }
//                    } else if constexpr (is_same_v<T, double>) {
//                        __m256d va = _mm256_loadu_pd(data1 + i);
//                        __m256d vb = _mm256_loadu_pd(data2 + i);
//                        __m256i vc = _mm256_castpd_si256(_mm256_cmp_pd(va, vb, _CMP_EQ_OQ));
//                        if (_mm256_movemask_epi8(vc) != 0xFFFFFFFF) {
//                            return false;
//                        }
//                    } else if constexpr (is_same_v<T, int> || is_same_v<T, unsigned> || is_same_v<T, short>) {
//                        __m256i va = _mm256_loadu_si256((__m256i *) (data1 + i));
//                        __m256i vb = _mm256_loadu_si256((__m256i *) (data2 + i));
//                        __m256i vc = _mm256_cmpeq_epi32(va, vb);
//                        if (_mm256_movemask_epi8(vc) != 0xFFFFFFFF) {
//                            return false;
//                        }
//                    } else {
//                        throw runtime_error("SIMD version not supported for this type");
//                    }
//                }
//                return true;
//            };
//            
//        }

    static inline constexpr T sum(const T* data1, const STLKR_SIMD &simdPolicy) {
        const auto &avxVersion = simdPolicy.getAVXType();
        if (avxVersion == SIMD_AVXType::AVX || avxVersion == SIMD_AVXType::AVX_2) {
            auto registerSize = STLKR_SIMD_DataTypeSize.at(type_index(typeid(T)));
            auto blockSize = registerSize * (size / registerSize); // Calculate full blocks that fit into the size

            auto simdSumJob = [&](unsigned start, unsigned end) {
                // Ensure the starting and ending points are aligned for SIMD operations.
                start = (start / registerSize) * registerSize;
                end = (end < size ? (end / registerSize) * registerSize : size);
                
                for (unsigned i = start; i < end; i += registerSize) {
                    if constexpr (is_same_v<T, float>) {
                        __m256 va = _mm256_loadu_ps(data1 + i);
                        __m256 vb = _mm256_permute2f128_ps(va, va, 1);
                        __m256 vc = _mm256_add_ps(va, vb);
                        __m128 vd = _mm256_extractf128_ps(vc, 1);
                        __m128 ve = _mm_add_ps(_mm256_castps256_ps128(vc), vd);
                        T sum = _mm_cvtss_f32(ve);
                        return sum;
                    } else if constexpr (is_same_v<T, double>) {
                        __m256d va = _mm256_loadu_pd(data1 + i);
                        __m256d vb = _mm256_permute2f128_pd(va, va, 1);
                        __m256d vc = _mm256_add_pd(va, vb);
                        __m128d vd = _mm256_extractf128_pd(vc, 1);
                        __m128d ve = _mm_add_pd(_mm256_castpd256_pd128(vc), vd);
                        T sum = _mm_cvtsd_f64(ve);
                        return sum;
                    } else if constexpr (is_same_v<T, int> || is_same_v<T, unsigned> || is_same_v<T, short>) {
                        __m256i va = _mm256_loadu_si256((__m256i *) (data1 + i));
                        __m256i vb = _mm256_permute2f128_si256(va, va, 1);
                        __m256i vc = _mm256_add_epi32(va, vb);
                        __m128i vd = _mm256_extractf128_si256(vc, 1);
                        __m128i ve = _mm_add_epi32(_mm256_castsi256_si128(vc), vd);
                        T sum = _mm_cvtsi128_si32(ve);
                        return sum;
                    } else {
                        throw runtime_error("SIMD version not supported for this type");
                    }
                }
            };
            return STLKR_ThreadingOperations<availableThreads>::executeParallelJobWithReduction(simdSumJob, size);
    }

    static inline constexpr void add(const T* data1, T scale1, const T* data2, T scale2, T* result) {
        auto addJob = [&](unsigned start, unsigned end) {
            for (unsigned i = start; i < end; i++) {
                result[i] = data1[i] * scale1 + data2[i] * scale2;
            }
        };
        STLKR_ThreadingOperations<availableThreads>::executeParallelJob(addJob, size);
    }


};// STLKR_LinearAlgebra
#endif //STALKER_STLKR_OPERATIONS_SIMD_H
