#pragma once
#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDBase.h>

namespace Stalker::Core
{
    // AVX512 Specialization for doubles
    template<>
    struct TypeTraitsSIMD<double, SIMDType::AVX512>
        : TypeTraitsSIMDBase<double, SIMDType::AVX512, TypeTraitsSIMD<double, SIMDType::AVX512>> {
        using Base = TypeTraitsSIMDBase<double, SIMDType::AVX512, TypeTraitsSIMD<double, SIMDType::AVX512>>;
        using typeSIMD = __m512d;
        using typeData = double;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 512 bits / 64 bits per double
    };

    // AVX512 Specialization for floats
    template<>
    struct TypeTraitsSIMD<float, SIMDType::AVX512> : TypeTraitsSIMDBase<float, SIMDType::AVX512, TypeTraitsSIMD<float, SIMDType::AVX512>> {
        using Base = TypeTraitsSIMDBase<float, SIMDType::AVX512, TypeTraitsSIMD<float, SIMDType::AVX512>>;
        using typeSIMD = __m512;
        using typeData = float;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16;  // 512 bits / 32 bits per float
    };

    // AVX512 Specialization for ints
    template<>
    struct TypeTraitsSIMD<int, SIMDType::AVX512> : TypeTraitsSIMDBase<int, SIMDType::AVX512, TypeTraitsSIMD<int, SIMDType::AVX512>> {
        using Base = TypeTraitsSIMDBase<int, SIMDType::AVX512, TypeTraitsSIMD<int, SIMDType::AVX512>>;
        using typeSIMD = __m512i;
        using typeData = int;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16;  // 512 bits / 32 bits per int
    };

    // AVX512 Specialization for shorts
    template<>
    struct TypeTraitsSIMD<short, SIMDType::AVX512> : TypeTraitsSIMDBase<short, SIMDType::AVX512, TypeTraitsSIMD<short, SIMDType::AVX512>> {
        using Base = TypeTraitsSIMDBase<short, SIMDType::AVX512, TypeTraitsSIMD<short, SIMDType::AVX512>>;
        using typeSIMD = __m512i;
        using typeData = short;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 32; // 512 bits / 16 bits per short
    };

    // AVX512 Specialization for unsigned ints
    template<>
    struct TypeTraitsSIMD<unsigned, SIMDType::AVX512> : TypeTraitsSIMDBase<unsigned, SIMDType::AVX512, TypeTraitsSIMD<unsigned, SIMDType::AVX512>> {
        using Base = TypeTraitsSIMDBase<unsigned, SIMDType::AVX512, TypeTraitsSIMD<unsigned, SIMDType::AVX512>>;
        using typeSIMD = __m512i;
        using typeData = unsigned;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16;  // 512 bits / 32 bits per unsigned int
    };
}