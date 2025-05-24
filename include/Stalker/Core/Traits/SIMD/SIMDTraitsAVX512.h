#pragma once
#include <Stalker/Core/Traits/SIMD/SIMDTraitsBase.h>

namespace Stalker::Core
{
    // AVX512 Specialization for doubles
    template<>
    struct SIMDTypeTraits<double, SIMDType::AVX512>
        : SIMDTypeTraitsBase<double, SIMDType::AVX512, SIMDTypeTraits<double, SIMDType::AVX512>> {
        using Base = SIMDTypeTraitsBase<double, SIMDType::AVX512, SIMDTypeTraits<double, SIMDType::AVX512>>;
        using typeSIMD = __m512d;
        using typeData = double;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 512 bits / 64 bits per double
    };

    // AVX512 Specialization for floats
    template<>
    struct SIMDTypeTraits<float, SIMDType::AVX512> : SIMDTypeTraitsBase<float, SIMDType::AVX512, SIMDTypeTraits<float, SIMDType::AVX512>> {
        using Base = SIMDTypeTraitsBase<float, SIMDType::AVX512, SIMDTypeTraits<float, SIMDType::AVX512>>;
        using typeSIMD = __m512;
        using typeData = float;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16;  // 512 bits / 32 bits per float
    };

    // AVX512 Specialization for ints
    template<>
    struct SIMDTypeTraits<int, SIMDType::AVX512> : SIMDTypeTraitsBase<int, SIMDType::AVX512, SIMDTypeTraits<int, SIMDType::AVX512>> {
        using Base = SIMDTypeTraitsBase<int, SIMDType::AVX512, SIMDTypeTraits<int, SIMDType::AVX512>>;
        using typeSIMD = __m512i;
        using typeData = int;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16;  // 512 bits / 32 bits per int
    };

    // AVX512 Specialization for shorts
    template<>
    struct SIMDTypeTraits<short, SIMDType::AVX512> : SIMDTypeTraitsBase<short, SIMDType::AVX512, SIMDTypeTraits<short, SIMDType::AVX512>> {
        using Base = SIMDTypeTraitsBase<short, SIMDType::AVX512, SIMDTypeTraits<short, SIMDType::AVX512>>;
        using typeSIMD = __m512i;
        using typeData = short;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 32; // 512 bits / 16 bits per short
    };

    // AVX512 Specialization for unsigned ints
    template<>
    struct SIMDTypeTraits<unsigned, SIMDType::AVX512> : SIMDTypeTraitsBase<unsigned, SIMDType::AVX512, SIMDTypeTraits<unsigned, SIMDType::AVX512>> {
        using Base = SIMDTypeTraitsBase<unsigned, SIMDType::AVX512, SIMDTypeTraits<unsigned, SIMDType::AVX512>>;
        using typeSIMD = __m512i;
        using typeData = unsigned;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16;  // 512 bits / 32 bits per unsigned int
    };
}