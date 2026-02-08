/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDBase.h>

namespace Stalker::Core
{
    // AVX512 Specialization for doubles
    template<>
    struct TypeTraitsSIMD<double, T_SIMD::AVX512>
        : TypeTraitsSIMDBase<double, T_SIMD::AVX512, TypeTraitsSIMD<double, T_SIMD::AVX512>> {
        using Base = TypeTraitsSIMDBase<double, T_SIMD::AVX512, TypeTraitsSIMD<double, T_SIMD::AVX512>>;
        using typeSIMD     = __m512d;
        using typeSIMDHalf = __m256d;
        using typeData     = double;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 8;  // 512 bits / 64 bits per double
    };

    // AVX512 Specialization for floats
    template<>
    struct TypeTraitsSIMD<float, T_SIMD::AVX512> : TypeTraitsSIMDBase<float, T_SIMD::AVX512, TypeTraitsSIMD<float, T_SIMD::AVX512>> {
        using Base = TypeTraitsSIMDBase<float, T_SIMD::AVX512, TypeTraitsSIMD<float, T_SIMD::AVX512>>;
        using typeSIMD     = __m512;
        using typeSIMDHalf = __m256;
        using typeData     = float;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16;  // 512 bits / 32 bits per float
    };

    // AVX512 Specialization for ints
    template<>
    struct TypeTraitsSIMD<int, T_SIMD::AVX512> : TypeTraitsSIMDBase<int, T_SIMD::AVX512, TypeTraitsSIMD<int, T_SIMD::AVX512>> {
        using Base = TypeTraitsSIMDBase<int, T_SIMD::AVX512, TypeTraitsSIMD<int, T_SIMD::AVX512>>;
        using typeSIMD     = __m512i;
        using typeSIMDHalf = __m256i;
        using typeData     = int;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16;  // 512 bits / 32 bits per int
    };

    // AVX512 Specialization for shorts
    template<>
    struct TypeTraitsSIMD<short, T_SIMD::AVX512> : TypeTraitsSIMDBase<short, T_SIMD::AVX512, TypeTraitsSIMD<short, T_SIMD::AVX512>> {
        using Base = TypeTraitsSIMDBase<short, T_SIMD::AVX512, TypeTraitsSIMD<short, T_SIMD::AVX512>>;
        using typeSIMD     = __m512i;
        using typeSIMDHalf = __m256i;
        using typeData     = short;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 32; // 512 bits / 16 bits per short
    };

    // AVX512 Specialization for unsigned ints
    template<>
    struct TypeTraitsSIMD<unsigned, T_SIMD::AVX512> : TypeTraitsSIMDBase<unsigned, T_SIMD::AVX512, TypeTraitsSIMD<unsigned, T_SIMD::AVX512>> {
        using Base = TypeTraitsSIMDBase<unsigned, T_SIMD::AVX512, TypeTraitsSIMD<unsigned, T_SIMD::AVX512>>;
        using typeSIMD     = __m512i;
        using typeSIMDHalf = __m256i;
        using typeData     = unsigned;
    private:
        friend Base;
        static constexpr unsigned _RegisterSize = 16;  // 512 bits / 32 bits per unsigned int
    };
}