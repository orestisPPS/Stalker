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


    static constexpr inline void addAVXRegister(const AVXRegisterType* data1, const AVXRegisterType* data2, AVXRegisterType* result,
                                                const AVXRegisterType* scalar1, const AVXRegisterType* scalar2) {
        _addAVXRegister<unrollFactor>(data1, data2, result, scalar1, scalar2);
    }

    static constexpr inline void storeAVXRegisterTemporal(const AVXRegisterType* source, DataType* destination) {
        _storeAVXRegisterTemporal<unrollFactor>(source, destination);
    }

    static constexpr inline void storeAVXRegisterNonTemporal(const AVXRegisterType* source, DataType* destination) {
        _storeAVXRegisterNonTemporal<unrollFactor>(source, destination);
    }

    static constexpr inline void setZeroAVXRegister(AVXRegisterType* destination) {
        _setZeroAVXRegister<unrollFactor>(destination);
    }

    static constexpr inline void setValue(AVXRegisterType* destination, DataType value) {
        _setValue<unrollFactor>(destination, value);
    }

    static constexpr inline bool areEqual(const AVXRegisterType* a, const AVXRegisterType* b) {
        return _areEqual<unrollFactor>(a, b);
    }

private:
    template <unsigned iUnroll>
    static constexpr inline void _fmaddAVXRegister(const AVXRegisterType* data1, const AVXRegisterType* data2, AVXRegisterType* result,
                                                  const AVXRegisterType* scalar1, const AVXRegisterType* scalar2) {
        if constexpr (iUnroll > 0) {
            //add with _mm256_fmadd
            *(result + iUnroll - 1) = _mm256_fmadd_ps(*(data1 + iUnroll - 1), *(data2 + iUnroll - 1), *(scalar1 + iUnroll - 1));
            _fmaddAVXRegister<iUnroll - 1>(data1, data2, result, scalar1, scalar2);
            
        }
    }


};
#endif //STALKER_AVX_MATHTRAITS_H
