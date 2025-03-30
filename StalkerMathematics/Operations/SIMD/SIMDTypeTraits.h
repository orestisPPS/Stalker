#ifndef SIMDTYPETRAITS_H
#define SIMDTYPETRAITS_H

#include <immintrin.h>
#include "../../../GlobalDefinitions.h"

enum class SIMDType {
    SSE,
    AVX,
    AVX2,
    AVX512
};

template<typename T, SIMDType Type, typename Child>
struct SIMDTypeTraitsBase {

    static constexpr unsigned RegisterSize = Child::_RegisterSize;
    static constexpr unsigned ElementsPerCacheLine = 64 / sizeof(typename Child::typeData);
    static constexpr unsigned CacheLinesProcessed = (UnrollFactorSIMD * RegisterSize) / ElementsPerCacheLine;
    static constexpr unsigned BlockSize = RegisterSize * UnrollFactorSIMD;
};


// AVX2 register sizes in terms of elements per register
inline constexpr unsigned DOUBLE_AVX2_REGISTER_SIZE   = 4;  // 256 bits / 64 bits per double
inline constexpr unsigned FLOAT_AVX2_REGISTER_SIZE    = 8;  // 256 bits / 32 bits per float
inline constexpr unsigned INT_AVX2_REGISTER_SIZE      = 8;  // 256 bits / 32 bits per int
inline constexpr unsigned SHORT_AVX2_REGISTER_SIZE    = 16; // 256 bits / 16 bits per short
inline constexpr unsigned UNSIGNED_AVX2_REGISTER_SIZE = 8;  // 256 bits / 32 bits per unsigned int


template<typename T, SIMDType Type> struct SIMDTypeTraits; 
template<> struct SIMDTypeTraits<double, SIMDType::AVX2>;
template<> struct SIMDTypeTraits<float, SIMDType::AVX2>;
template<> struct SIMDTypeTraits<int, SIMDType::AVX2>;
template<> struct SIMDTypeTraits<short, SIMDType::AVX2>;
template<> struct SIMDTypeTraits<unsigned, SIMDType::AVX2>;

// AVX2 Specialization for doubles
template<> struct SIMDTypeTraits<double, SIMDType::AVX2> : SIMDTypeTraitsBase<double, SIMDType::AVX2, SIMDTypeTraits<double, SIMDType::AVX2>> {
    using Base = SIMDTypeTraitsBase<double, SIMDType::AVX2, SIMDTypeTraits<double, SIMDType::AVX2>>;
    using typeSIMD = __m256d;
    using typeData = double;
private:
    friend Base;
    static constexpr unsigned _RegisterSize = DOUBLE_AVX2_REGISTER_SIZE;
};

// AVX2 Specialization for floats
template<> struct SIMDTypeTraits<float, SIMDType::AVX2> : SIMDTypeTraitsBase<float, SIMDType::AVX2, SIMDTypeTraits<float, SIMDType::AVX2>> {
    using Base = SIMDTypeTraitsBase<float, SIMDType::AVX2, SIMDTypeTraits<float, SIMDType::AVX2>>;
    using typeSIMD = __m256;
    using typeData = float;
private:
    friend Base;
    static constexpr unsigned _RegisterSize = FLOAT_AVX2_REGISTER_SIZE;
};

// AVX2 Specialization for ints
template<> struct SIMDTypeTraits<int, SIMDType::AVX2> : SIMDTypeTraitsBase<int, SIMDType::AVX2, SIMDTypeTraits<int, SIMDType::AVX2>> {
    using Base = SIMDTypeTraitsBase<int, SIMDType::AVX2, SIMDTypeTraits<int, SIMDType::AVX2>>;
    using typeSIMD = __m256i;
    using typeData = int;
private:
    friend Base;
    static constexpr unsigned _RegisterSize = INT_AVX2_REGISTER_SIZE;
};

// AVX2 Specialization for shorts
template<> struct SIMDTypeTraits<short, SIMDType::AVX2> : SIMDTypeTraitsBase<short, SIMDType::AVX2, SIMDTypeTraits<short, SIMDType::AVX2>> {
    using Base = SIMDTypeTraitsBase<short, SIMDType::AVX2, SIMDTypeTraits<short, SIMDType::AVX2>>;
    using typeSIMD = __m256i;
    using typeData = short;
private:
    friend Base;
    static constexpr unsigned _RegisterSize = SHORT_AVX2_REGISTER_SIZE;
};

// AVX2 Specialization for unsigned ints
template<> struct SIMDTypeTraits<unsigned, SIMDType::AVX2> : SIMDTypeTraitsBase<unsigned, SIMDType::AVX2, SIMDTypeTraits<unsigned, SIMDType::AVX2>> {
    using Base = SIMDTypeTraitsBase<unsigned, SIMDType::AVX2, SIMDTypeTraits<unsigned, SIMDType::AVX2>>;
    using typeSIMD = __m256i;
    using typeData = unsigned;
private:
    friend Base;
    static constexpr unsigned _RegisterSize = UNSIGNED_AVX2_REGISTER_SIZE;
};



#endif // SIMDTYPETRAITS_H