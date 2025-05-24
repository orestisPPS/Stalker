#pragma once

#include <Stalker/Core/Traits/SIMD/SIMDTraitsBase.h>

/**
 * @file SIMDTraitsAVX2.h
 * @brief Specializations of SIMDTypeTraits for AVX2 SIMDType.
 *
 * This file provides template specializations of the SIMDTypeTraits struct for the supported fundamental types
 * (double, float, int, short, unsigned) using the AVX2 instruction set. Each specialization defines the
 * appropriate SIMD register type, the underlying data type, and the register size (number of elements per register).
 *
 * These specializations inherit from SIMDTypeTraitsBase using the Curiously Recurring Template Pattern (CRTP),
 * which allows the base class to access members of the derived class at compile time, enabling static polymorphism
 * and compile-time optimizations.
 *
 * @tparam T The fundamental data type (double, float, int, short, unsigned).
 * @tparam SIMDType The SIMD instruction set type (here, always SIMDType::AVX2).
 *
 * @note
 * - typeSIMD: The AVX2 SIMD register type for the given data type.
 * - typeData: The underlying scalar data type.
 * - _RegisterSize: Number of elements of typeData that fit in a 256-bit AVX2 register.
 *
 * @see SIMDTypeTraitsBase
 * @see SIMDType
 */

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