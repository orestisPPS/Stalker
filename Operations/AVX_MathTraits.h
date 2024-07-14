//
// Created by hal9000 on 7/9/24.
//

#ifndef STALKER_AVX_MATHTRAITS_H
#define STALKER_AVX_MATHTRAITS_H
#include "AVX_MemoryTraits.h"
#include <immintrin.h>

template<typename T, unsigned int unrollFactor>
struct AVX_MathTraits;

// Float Specialization
template<unsigned int unrollFactor>
struct AVX_MathTraits<float, unrollFactor> {
    using AVXRegisterType = __m256;
    using DataType = float;
    static constexpr unsigned AVXRegisterSize = FLOAT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(float);

    static constexpr inline void add(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollAdd<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void subtract(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollSubtract<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void multiply(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollMultiply<unrollFactor>(a, b, result, scale1, scale2);
    }

private:
    template<unsigned iUnroll>
    static constexpr inline void _unrollAdd(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_fmadd_ps(*(a + iUnroll - 1), *scale1, _mm256_mul_ps(*(b + iUnroll - 1), *scale2));
            _unrollAdd<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollSubtract(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_fmsub_ps(*(a + iUnroll - 1), *scale1, _mm256_mul_ps(*(b + iUnroll - 1), *scale2));
            _unrollSubtract<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollMultiply(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_mul_ps(_mm256_mul_ps(*(a + iUnroll - 1), *scale1), _mm256_mul_ps(*(b + iUnroll - 1), *scale2));
            _unrollMultiply<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }
};

// Double Specialization
template<unsigned int unrollFactor>
struct AVX_MathTraits<double, unrollFactor> {
    using AVXRegisterType = __m256d;
    using DataType = double;
    static constexpr unsigned AVXRegisterSize = DOUBLE_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(double);

    static constexpr inline void add(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollAdd<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void subtract(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollSubtract<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void multiply(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollMultiply<unrollFactor>(a, b, result, scale1, scale2);
    }

private:
    template<unsigned iUnroll>
    static constexpr inline void _unrollAdd(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_fmadd_pd(*(a + iUnroll - 1), *scale1, _mm256_mul_pd(*(b + iUnroll - 1), *scale2));
            _unrollAdd<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollSubtract(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_fmsub_pd(*(a + iUnroll - 1), *scale1, _mm256_mul_pd(*(b + iUnroll - 1), *scale2));
            _unrollSubtract<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollMultiply(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_mul_pd(_mm256_mul_pd(*(a + iUnroll - 1), *scale1), _mm256_mul_pd(*(b + iUnroll - 1), *scale2));
            _unrollMultiply<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }
};

// Int Specialization
template<unsigned int unrollFactor>
struct AVX_MathTraits<int, unrollFactor> {
    using AVXRegisterType = __m256i;
    using DataType = int;
    static constexpr unsigned AVXRegisterSize = INT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(int);

    static constexpr inline void add(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollAdd<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void subtract(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollSubtract<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void multiply(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollMultiply<unrollFactor>(a, b, result, scale1, scale2);
    }

private:
    template<unsigned iUnroll>
    static constexpr inline void _unrollAdd(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_add_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollAdd<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollSubtract(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_sub_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollSubtract<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollMultiply(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_mullo_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollMultiply<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }
};

// Unsigned Specialization
template<unsigned int unrollFactor>
struct AVX_MathTraits<unsigned, unrollFactor> {
    using AVXRegisterType = __m256i;
    using DataType = unsigned;
    static constexpr unsigned AVXRegisterSize = UNSIGNED_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(unsigned);

    static constexpr inline void add(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollAdd<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void subtract(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollSubtract<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void multiply(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollMultiply<unrollFactor>(a, b, result, scale1, scale2);
    }

private:
    template<unsigned iUnroll>
    static constexpr inline void _unrollAdd(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_add_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollAdd<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollSubtract(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_sub_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollSubtract<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollMultiply(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_mullo_epi32(_mm256_mullo_epi32(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi32(*(b + iUnroll - 1), *scale2));
            _unrollMultiply<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }
};

// Short Specialization
template<unsigned int unrollFactor>
struct AVX_MathTraits<short, unrollFactor> {
    using AVXRegisterType = __m256i;
    using DataType = short;
    static constexpr unsigned AVXRegisterSize = SHORT_AVX_REGISTER_SIZE;
    static constexpr unsigned elementsPerCacheLine = 64 / sizeof(short);

    static constexpr inline void add(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollAdd<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void subtract(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollSubtract<unrollFactor>(a, b, result, scale1, scale2);
    }

    static constexpr inline void multiply(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        _unrollMultiply<unrollFactor>(a, b, result, scale1, scale2);
    }

private:
    template<unsigned iUnroll>
    static constexpr inline void _unrollAdd(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_add_epi16(_mm256_mullo_epi16(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi16(*(b + iUnroll - 1), *scale2));
            _unrollAdd<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollSubtract(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_sub_epi16(_mm256_mullo_epi16(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi16(*(b + iUnroll - 1), *scale2));
            _unrollSubtract<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }

    template<unsigned iUnroll>
    static constexpr inline void _unrollMultiply(const AVXRegisterType* a, const AVXRegisterType* b, AVXRegisterType* result, AVXRegisterType* scale1, AVXRegisterType* scale2) {
        if constexpr (iUnroll > 0) {
            *(result + iUnroll - 1) = _mm256_mullo_epi16(_mm256_mullo_epi16(*(a + iUnroll - 1), *scale1), _mm256_mullo_epi16(*(b + iUnroll - 1), *scale2));
            _unrollMultiply<iUnroll - 1>(a, b, result, scale1, scale2);
        }
    }
};
#endif //STALKER_AVX_MATHTRAITS_H
