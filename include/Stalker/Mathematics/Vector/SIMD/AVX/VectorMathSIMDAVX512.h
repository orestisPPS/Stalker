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

#include <Stalker/Mathematics/Vector/SIMD/VectorMathSIMDBase.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX512.h>

namespace Stalker::Mathematics {

    template <>
    struct VectorMathSIMD<double, T_SIMD::AVX512>
            : public VectorMathSIMDBase<double, T_SIMD::AVX512, VectorMathSIMD<double, T_SIMD::AVX512>> {

        using T_simd = typename TypeTraitsSIMD<double, T_SIMD::AVX512>::typeSIMD;
        using T_data = typename TypeTraitsSIMD<double, T_SIMD::AVX512>::typeData;

    private:
        
        friend VectorMathSIMDBase<double, T_SIMD::AVX512, VectorMathSIMD<double, T_SIMD::AVX512>>;

        STALKER_FORCE_INLINE static T_simd _add(const T_simd& a, const T_simd& b) {
            return _mm512_add_pd(a, b);
        }

        STALKER_FORCE_INLINE static T_simd _subtract(const T_simd& a, const T_simd& b) {
            return _mm512_sub_pd(a, b);
        }

        STALKER_FORCE_INLINE static T_simd _multiply(const T_simd& a, const T_simd& b) {
            return _mm512_mul_pd(a, b);
        }

        STALKER_FORCE_INLINE static T_simd _axpy(const T_simd& a, const T_simd& b, const T_simd& scalar) {
            return _mm512_fmadd_pd(a, scalar, b);
        }

        STALKER_FORCE_INLINE static T_simd _axmy(const T_simd& a, const T_simd& b, const T_simd& scalar) {
            return _mm512_fmsub_pd(a, scalar, b);
        }
    };

    template <>
    struct VectorMathSIMD<float, T_SIMD::AVX512>
            : public VectorMathSIMDBase<float, T_SIMD::AVX512, VectorMathSIMD<float, T_SIMD::AVX512>> {

        using T_simd = typename TypeTraitsSIMD<float, T_SIMD::AVX512>::typeSIMD;
        using T_data = typename TypeTraitsSIMD<float, T_SIMD::AVX512>::typeData;

    private:
        friend VectorMathSIMDBase<float, T_SIMD::AVX512, VectorMathSIMD<float, T_SIMD::AVX512>>;

        STALKER_FORCE_INLINE static T_simd _add(const T_simd& a, const T_simd& b) {
            return _mm512_add_ps(a, b);
        }

        STALKER_FORCE_INLINE static T_simd _subtract(const T_simd& a, const T_simd& b) {
            return _mm512_sub_ps(a, b);
        }

        STALKER_FORCE_INLINE static T_simd _multiply(const T_simd& a, const T_simd& b) {
            return _mm512_mul_ps(a, b);
        }

        STALKER_FORCE_INLINE static T_simd _axpy(const T_simd& a, const T_simd& b, const T_simd& scalar) {
            return _mm512_fmadd_ps(a, scalar, b);
        }

        STALKER_FORCE_INLINE static T_simd _axmy(const T_simd& a, const T_simd& b, const T_simd& scalar) {
            return _mm512_fmsub_ps(a, scalar, b);
        }
    };

template<>
struct VectorMathSIMD<int, T_SIMD::AVX512>
        : public VectorMathSIMDBase<int, T_SIMD::AVX512, VectorMathSIMD<int, T_SIMD::AVX512>> {
    
        using T_simd = typename TypeTraitsSIMD<int, T_SIMD::AVX512>::typeSIMD;
        using T_data = typename TypeTraitsSIMD<int, T_SIMD::AVX512>::typeData;

private:
    
    friend VectorMathSIMDBase<int, T_SIMD::AVX512, VectorMathSIMD<int, T_SIMD::AVX512>>;

    STALKER_FORCE_INLINE static T_simd _add(const T_simd& a, const T_simd& b) {
        return _mm512_add_epi32(a, b);
    }

    STALKER_FORCE_INLINE static T_simd _subtract(const T_simd& a, const T_simd& b) {
        return _mm512_sub_epi32(a, b);
    }

    STALKER_FORCE_INLINE static T_simd _multiply(const T_simd& a, const T_simd& b) {
        return _mm512_mullo_epi32(a, b);
    }

    STALKER_FORCE_INLINE static T_simd _axpy(const T_simd& a, const T_simd& b, const T_simd& scalar) {
        return _mm512_add_epi32(_mm512_mullo_epi32(a, scalar), b);
    }

    STALKER_FORCE_INLINE static T_simd _axmy(const T_simd& a, const T_simd& b, const T_simd& scalar) {
        return _mm512_sub_epi32(_mm512_mullo_epi32(a, scalar), b);
    }
};

template<>
struct VectorMathSIMD<unsigned int, T_SIMD::AVX512>
        : public VectorMathSIMDBase<unsigned int, T_SIMD::AVX512, VectorMathSIMD<unsigned int, T_SIMD::AVX512>> {
    
        using T_simd = typename TypeTraitsSIMD<unsigned int, T_SIMD::AVX512>::typeSIMD;
        using T_data = typename TypeTraitsSIMD<unsigned int, T_SIMD::AVX512>::typeData;

private:

    friend VectorMathSIMDBase<unsigned int, T_SIMD::AVX512, VectorMathSIMD<unsigned int, T_SIMD::AVX512>>;

    STALKER_FORCE_INLINE static T_simd _add(const T_simd& a, const T_simd& b) {
        return _mm512_add_epi32(a, b);
    }


    STALKER_FORCE_INLINE static T_simd _subtract(const T_simd& a, const T_simd& b) {
        return _mm512_sub_epi32(a, b);
    }


    STALKER_FORCE_INLINE static T_simd _multiply(const T_simd& a, const T_simd& b) {
        return _mm512_mullo_epi32(a, b);
    }

    STALKER_FORCE_INLINE static T_simd _axpy(const T_simd& a, const T_simd& b, const T_simd& scalar) {
        return _mm512_add_epi32(_mm512_mullo_epi32(a, scalar), b);
    }

    STALKER_FORCE_INLINE static T_simd _axmy(const T_simd& a, const T_simd& b, const T_simd& scalar) {
        return _mm512_sub_epi32(_mm512_mullo_epi32(a, scalar), b);
    }
};

template<>
struct VectorMathSIMD<short, T_SIMD::AVX512>
        : public VectorMathSIMDBase<short, T_SIMD::AVX512, VectorMathSIMD<short, T_SIMD::AVX512>> {
    
        using T_simd = typename TypeTraitsSIMD<short, T_SIMD::AVX512>::typeSIMD;
        using T_data = typename TypeTraitsSIMD<short, T_SIMD::AVX512>::typeData;

private:
    
    friend VectorMathSIMDBase<short, T_SIMD::AVX512, VectorMathSIMD<short, T_SIMD::AVX512>>;

    STALKER_FORCE_INLINE static T_simd _add(const T_simd& a, const T_simd& b) {
        return _mm512_add_epi16(a, b);
    }

    STALKER_FORCE_INLINE static T_simd _subtract(const T_simd& a, const T_simd& b) {
        return _mm512_sub_epi16(a, b);
    }


    STALKER_FORCE_INLINE static T_simd _multiply(const T_simd& a, const T_simd& b) {
        return _mm512_mullo_epi16(a, b);
    }

    STALKER_FORCE_INLINE static T_simd _axpy(const T_simd& a, const T_simd& b, const T_simd& scalar) {
        return _mm512_add_epi16(_mm512_mullo_epi16(a, scalar), b);
    }

    STALKER_FORCE_INLINE static T_simd _axmy(const T_simd& a, const T_simd& b, const T_simd& scalar) {
        return _mm512_sub_epi16(_mm512_mullo_epi16(a, scalar), b);
    }
};



} // namespace Stalker::Mathematics

