#pragma once

#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDBase.h>

namespace Stalker::Core
{
    // AVX2 Specialization for doubles
    template<>
    struct TypeTraitsSIMD<double, SIMDType::AVX2>
        : TypeTraitsSIMDBase<double, SIMDType::AVX2, TypeTraitsSIMD<double, SIMDType::AVX2>> {
        using Base = TypeTraitsSIMDBase<double, SIMDType::AVX2, TypeTraitsSIMD<double, SIMDType::AVX2>>;
        using typeSIMD = __m256d;
        using typeData = double;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 4;  // 256 bits / 64 bits per double
    };

    // AVX2 Specialization for floats
    template<>
    struct TypeTraitsSIMD<float, SIMDType::AVX2> : TypeTraitsSIMDBase<float, SIMDType::AVX2, TypeTraitsSIMD<float, SIMDType::AVX2>> {
        using Base = TypeTraitsSIMDBase<float, SIMDType::AVX2, TypeTraitsSIMD<float, SIMDType::AVX2>>;
        using typeSIMD = __m256;
        using typeData = float;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 256 bits / 32 bits per float
    };

    // AVX2 Specialization for ints
    template<>
    struct TypeTraitsSIMD<int, SIMDType::AVX2> : TypeTraitsSIMDBase<int, SIMDType::AVX2, TypeTraitsSIMD<int, SIMDType::AVX2>> {
        using Base = TypeTraitsSIMDBase<int, SIMDType::AVX2, TypeTraitsSIMD<int, SIMDType::AVX2>>;
        using typeSIMD = __m256i;
        using typeData = int;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 256 bits / 32 bits per int
    };

    // AVX2 Specialization for shorts
    template<>
    struct TypeTraitsSIMD<short, SIMDType::AVX2> : TypeTraitsSIMDBase<short, SIMDType::AVX2, TypeTraitsSIMD<short, SIMDType::AVX2>> {
        using Base = TypeTraitsSIMDBase<short, SIMDType::AVX2, TypeTraitsSIMD<short, SIMDType::AVX2>>;
        using typeSIMD = __m256i;
        using typeData = short;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16; // 256 bits / 16 bits per short
    };

    // AVX2 Specialization for unsigned ints
    template<>
    struct TypeTraitsSIMD<unsigned, SIMDType::AVX2> : TypeTraitsSIMDBase<unsigned, SIMDType::AVX2, TypeTraitsSIMD<unsigned, SIMDType::AVX2>> {
        using Base = TypeTraitsSIMDBase<unsigned, SIMDType::AVX2, TypeTraitsSIMD<unsigned, SIMDType::AVX2>>;
        using typeSIMD = __m256i;
        using typeData = unsigned;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 256 bits / 32 bits per unsigned int
    };
}