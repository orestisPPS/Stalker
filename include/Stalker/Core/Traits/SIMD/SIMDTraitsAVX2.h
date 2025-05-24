#pragma once

#include <Stalker/Core/Traits/SIMD/SIMDTraitsBase.h>

namespace Stalker::Core
{
    // AVX2 Specialization for doubles
    template<>
    struct SIMDTypeTraits<double, SIMDType::AVX2>
        : SIMDTypeTraitsBase<double, SIMDType::AVX2, SIMDTypeTraits<double, SIMDType::AVX2>> {
        using Base = SIMDTypeTraitsBase<double, SIMDType::AVX2, SIMDTypeTraits<double, SIMDType::AVX2>>;
        using typeSIMD = __m256d;
        using typeData = double;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 4;  // 256 bits / 64 bits per double
    };

    // AVX2 Specialization for floats
    template<>
    struct SIMDTypeTraits<float, SIMDType::AVX2> : SIMDTypeTraitsBase<float, SIMDType::AVX2, SIMDTypeTraits<float, SIMDType::AVX2>> {
        using Base = SIMDTypeTraitsBase<float, SIMDType::AVX2, SIMDTypeTraits<float, SIMDType::AVX2>>;
        using typeSIMD = __m256;
        using typeData = float;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 256 bits / 32 bits per float
    };

    // AVX2 Specialization for ints
    template<>
    struct SIMDTypeTraits<int, SIMDType::AVX2> : SIMDTypeTraitsBase<int, SIMDType::AVX2, SIMDTypeTraits<int, SIMDType::AVX2>> {
        using Base = SIMDTypeTraitsBase<int, SIMDType::AVX2, SIMDTypeTraits<int, SIMDType::AVX2>>;
        using typeSIMD = __m256i;
        using typeData = int;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 256 bits / 32 bits per int
    };

    // AVX2 Specialization for shorts
    template<>
    struct SIMDTypeTraits<short, SIMDType::AVX2> : SIMDTypeTraitsBase<short, SIMDType::AVX2, SIMDTypeTraits<short, SIMDType::AVX2>> {
        using Base = SIMDTypeTraitsBase<short, SIMDType::AVX2, SIMDTypeTraits<short, SIMDType::AVX2>>;
        using typeSIMD = __m256i;
        using typeData = short;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16; // 256 bits / 16 bits per short
    };

    // AVX2 Specialization for unsigned ints
    template<>
    struct SIMDTypeTraits<unsigned, SIMDType::AVX2> : SIMDTypeTraitsBase<unsigned, SIMDType::AVX2, SIMDTypeTraits<unsigned, SIMDType::AVX2>> {
        using Base = SIMDTypeTraitsBase<unsigned, SIMDType::AVX2, SIMDTypeTraits<unsigned, SIMDType::AVX2>>;
        using typeSIMD = __m256i;
        using typeData = unsigned;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 256 bits / 32 bits per unsigned int
    };
}