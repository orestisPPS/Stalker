#pragma once

#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDBase.h>

namespace Stalker::Core
{
    // AVX2 Specialization for doubles
    template<>
    struct TypeTraitsSIMD<double, T_SIMD::AVX2>
        : TypeTraitsSIMDBase<double, T_SIMD::AVX2, TypeTraitsSIMD<double, T_SIMD::AVX2>> {
        using Base = TypeTraitsSIMDBase<double, T_SIMD::AVX2, TypeTraitsSIMD<double, T_SIMD::AVX2>>;
        using typeSIMD     = __m256d;
        using typeSIMDHalf = __m128d;
        using typeData     = double;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 4;  // 256 bits / 64 bits per double
    };

    // AVX2 Specialization for floats
    template<>
    struct TypeTraitsSIMD<float, T_SIMD::AVX2> : TypeTraitsSIMDBase<float, T_SIMD::AVX2, TypeTraitsSIMD<float, T_SIMD::AVX2>> {
        using Base = TypeTraitsSIMDBase<float, T_SIMD::AVX2, TypeTraitsSIMD<float, T_SIMD::AVX2>>;
        using typeSIMD     = __m256;
        using typeSIMDHalf = __m128;
        using typeData     = float;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 256 bits / 32 bits per float
    };

    // AVX2 Specialization for ints
    template<>
    struct TypeTraitsSIMD<int, T_SIMD::AVX2> : TypeTraitsSIMDBase<int, T_SIMD::AVX2, TypeTraitsSIMD<int, T_SIMD::AVX2>> {
        using Base = TypeTraitsSIMDBase<int, T_SIMD::AVX2, TypeTraitsSIMD<int, T_SIMD::AVX2>>;
        using typeSIMD     = __m256i;
        using typeSIMDHalf = __m128i;
        using typeData     = int;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 256 bits / 32 bits per int
    };

    // AVX2 Specialization for shorts
    template<>
    struct TypeTraitsSIMD<short, T_SIMD::AVX2> : TypeTraitsSIMDBase<short, T_SIMD::AVX2, TypeTraitsSIMD<short, T_SIMD::AVX2>> {
        using Base = TypeTraitsSIMDBase<short, T_SIMD::AVX2, TypeTraitsSIMD<short, T_SIMD::AVX2>>;
        using typeSIMD     = __m256i;
        using typeSIMDHalf = __m128i;
        using typeData     = short;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16; // 256 bits / 16 bits per short
    };

    // AVX2 Specialization for unsigned ints
    template<>
    struct TypeTraitsSIMD<unsigned, T_SIMD::AVX2> : TypeTraitsSIMDBase<unsigned, T_SIMD::AVX2, TypeTraitsSIMD<unsigned, T_SIMD::AVX2>> {
        using Base = TypeTraitsSIMDBase<unsigned, T_SIMD::AVX2, TypeTraitsSIMD<unsigned, T_SIMD::AVX2>>;
        using typeSIMD     = __m256i;
        using typeSIMDHalf = __m128i;
        using typeData     = unsigned;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 256 bits / 32 bits per unsigned int
    };
}