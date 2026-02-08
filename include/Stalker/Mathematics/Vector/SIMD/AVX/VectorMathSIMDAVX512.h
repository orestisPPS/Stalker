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

#include <Stalker/Core/Traits/TypeTraits/SIMD/TypeTraitsSIMDAVX512.h>
#include <Stalker/Mathematics/Vector/SIMD/VectorMathSIMDBase.h>
#include <Stalker/Memory/SIMD/MemoryOperationsSIMDAVX512.h>

namespace Stalker::Mathematics {

    template <>
    struct VectorMathSIMD<double, T_SIMD::AVX512>
            : public VectorMathSIMDBase<double, T_SIMD::AVX512, VectorMathSIMD<double, T_SIMD::AVX512>> {

        using Base = VectorMathSIMDBase<double, T_SIMD::AVX512, VectorMathSIMD<double, T_SIMD::AVX512>>;
        using Traits = TypeTraitsSIMD<double, T_SIMD::AVX512>;
        using T_simd = typename Traits::typeSIMD;
        using T_data = typename Traits::typeData;
        using Memory = MemoryOperationsSIMD<double, T_SIMD::AVX512>;

    private:
        friend Base;

        template <bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>,
                                const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::storeOffset<Is, Policy>(result, _mm512_fmadd_pd(Memory::loadOffset<Is, IsAligned>(a), *scalarA,
                                                           _mm512_mul_pd(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
            else
                (Memory::storeOffset<Is, Policy>(result, _mm512_add_pd(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
        static inline void _axpy(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>, const T_simd* scalar = nullptr) {
            (Memory::storeOffset<Is, Policy>(result, _mm512_fmadd_pd(Memory::loadOffset<Is, IsAligned>(a), *scalar, Memory::loadOffset<Is, IsAligned>(b))), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>,
                                     const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::storeOffset<Is, Policy>(result, _mm512_fmadd_pd(Memory::loadOffset<Is, IsAligned>(a), *scalarA,
                                                           _mm512_mul_pd(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
            else
                (Memory::storeOffset<Is, Policy>(result, _mm512_sub_pd(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>,
                                     const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::storeOffset<Is, Policy>(result, _mm512_mul_pd(_mm512_mul_pd(Memory::loadOffset<Is, IsAligned>(a), *scalarA),
                                                                       _mm512_mul_pd(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
            else
                (Memory::storeOffset<Is, Policy>(result, _mm512_mul_pd(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
        static inline void _scale(const T_data* data, T_data* result, const T_simd* scalar, std::index_sequence<Is...>) {
            (Memory::storeOffset<Is, Policy>(result, _mm512_mul_pd(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict T_data* data, const T_simd* scalar, std::index_sequence<Is...>) {
            (Memory::storeOffset<Is, Policy>(data, _mm512_mul_pd(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const T_data* data, T_data* result, const T_simd* scalar, std::index_sequence<Is...>) {
            (Memory::storeOffset<Is, Policy>(result, _mm512_add_pd(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict T_data* data, const T_simd* scalar, std::index_sequence<Is...>) {
            (Memory::storeOffset<Is, Policy>(data, _mm512_add_pd(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
        }

        template <bool IsAligned, size_t... Is>
        static inline void _sum(const T_data* __restrict data, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm512_add_pd(accumulators[Is], Memory::loadOffset<Is, IsAligned>(data))), ...);
        }

        template <bool IsAligned, size_t... Is>
        static inline void _dot(const T_data __restrict* a, const T_data __restrict* b, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm512_fmadd_pd(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b), accumulators[Is])), ...);
        }
    };

    template <>
    struct VectorMathSIMD<float, T_SIMD::AVX512>
            : public VectorMathSIMDBase<float, T_SIMD::AVX512, VectorMathSIMD<float, T_SIMD::AVX512>> {

        using Base = VectorMathSIMDBase<float, T_SIMD::AVX512, VectorMathSIMD<float, T_SIMD::AVX512>>;
        using Traits = TypeTraitsSIMD<float, T_SIMD::AVX512>;
        using T_simd = typename Traits::typeSIMD;
        using T_data = typename Traits::typeData;
        using Memory = MemoryOperationsSIMD<float, T_SIMD::AVX512>;

    private:
        friend Base;

        template <bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _add(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>,
                                const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::storeOffset<Is, Policy>(result, _mm512_fmadd_ps(Memory::loadOffset<Is, IsAligned>(a), *scalarA,
                                                           _mm512_mul_ps(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
            else
                (Memory::storeOffset<Is, Policy>(result, _mm512_add_ps(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
        static inline void _axpy(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>, const T_simd* scalar = nullptr) {
            (Memory::storeOffset<Is, Policy>(result, _mm512_fmadd_ps(Memory::loadOffset<Is, IsAligned>(a), *scalar, Memory::loadOffset<Is, IsAligned>(b))), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _subtract(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>,
                                     const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::storeOffset<Is, Policy>(result, _mm512_fmadd_ps(Memory::loadOffset<Is, IsAligned>(a), *scalarA,
                                                           _mm512_mul_ps(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
            else
                (Memory::storeOffset<Is, Policy>(result, _mm512_sub_ps(Memory::loadOffset<Is, IsAligned>(a),  Memory::loadOffset<Is, IsAligned>(b))), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
        static inline void _multiply(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>,
                                     const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
            if constexpr (IsScaled)
                (Memory::storeOffset<Is, Policy>(result, _mm512_mul_ps(_mm512_mul_ps(Memory::loadOffset<Is, IsAligned>(a), *scalarA),
                                                                       _mm512_mul_ps(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
            else
                (Memory::storeOffset<Is, Policy>(result, _mm512_mul_ps(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
        static inline void _scale(const T_data* data, T_data* result, const T_simd* scalar, std::index_sequence<Is...>) {
            (Memory::storeOffset<Is, Policy>(result, _mm512_mul_ps(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
        static inline void _scale(__restrict T_data* data, const T_simd* scalar, std::index_sequence<Is...>) {
            (Memory::storeOffset<Is, Policy>(data, _mm512_mul_ps(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(const T_data* data, T_data* result, const T_simd* scalar, std::index_sequence<Is...>) {
            (Memory::storeOffset<Is, Policy>(result, _mm512_add_ps(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
        }

        template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
        static inline void _addConstant(__restrict T_data* data, const T_simd* scalar, std::index_sequence<Is...>) {
            (Memory::storeOffset<Is, Policy>(data, _mm512_add_ps(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
        }

        template <bool IsAligned, size_t... Is>
        static inline void _sum(const T_data* __restrict data, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm512_add_ps(accumulators[Is], Memory::loadOffset<Is, IsAligned>(data))), ...);
        }

        template <bool IsAligned, size_t... Is>
        static inline void _dot(const T_data __restrict* a, const T_data __restrict* b, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
            ((accumulators[Is] = _mm512_fmadd_ps(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b), accumulators[Is])), ...);
        }
    };

template<>
struct VectorMathSIMD<int, T_SIMD::AVX512>
        : public VectorMathSIMDBase<int, T_SIMD::AVX512, VectorMathSIMD<int, T_SIMD::AVX512>> {
    
    using Base = VectorMathSIMDBase<int, T_SIMD::AVX512, VectorMathSIMD<int, T_SIMD::AVX512>>;
    using Traits = TypeTraitsSIMD<int, T_SIMD::AVX512>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;
    using Memory = MemoryOperationsSIMD<int, T_SIMD::AVX512>;

private:
    
    friend Base;

    template<bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _add(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>, const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
        if constexpr (IsScaled) {
            (Memory::storeOffset<Is, Policy>(result, _mm512_add_epi32(_mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a), *scalarA),
                                                                      _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
        }
        else {
            (Memory::storeOffset<Is, Policy>(result, _mm512_add_epi32(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))), ...);
        }
    }

    template<bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _axpy(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>, const T_simd* scalar = nullptr) {
        (Memory::storeOffset<Is, Policy>(result, _mm512_add_epi32(_mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a), *scalar),
                            Memory::loadOffset<Is, IsAligned>(b))), ...);
    }

    template<bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>, const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Memory::storeOffset<Is, Policy>(result, _mm512_sub_epi32(_mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a), *scalarA),
                                                                      _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
        else
            (Memory::storeOffset<Is, Policy>(result, _mm512_sub_epi32(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))), ...);
    }

    template<bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>, const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Memory::storeOffset<Is, Policy>(
                result, _mm512_mullo_epi32(_mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a), *scalarA),
                                           _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
        else
            (Memory::storeOffset<Is, Policy>(result, _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _scale(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(result, _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }
    
    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _scale(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(data, _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(result, _mm512_add_epi32(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(data, _mm512_add_epi32(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }

    template <bool IsAligned, size_t... Is>
    static inline void _sum(const T_data* __restrict data, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_add_epi32(accumulators[Is], Memory::loadOffset<Is, IsAligned>(data))), ...);
    }
    
    template <bool IsAligned, size_t... Is>
    static inline void _dot(const T_data __restrict *a, const T_data __restrict *b, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_add_epi32(accumulators[Is],_mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a),
                                                                                  Memory::loadOffset<Is, IsAligned>(b)))), ...);
    }
};

template<>
struct VectorMathSIMD<unsigned int, T_SIMD::AVX512>
        : public VectorMathSIMDBase<unsigned int, T_SIMD::AVX512, VectorMathSIMD<unsigned int, T_SIMD::AVX512>> {
    
    using Base = VectorMathSIMDBase<unsigned int, T_SIMD::AVX512, VectorMathSIMD<unsigned int, T_SIMD::AVX512>>;
    using Traits = TypeTraitsSIMD<unsigned int, T_SIMD::AVX512>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;
    using Memory = MemoryOperationsSIMD<unsigned int, T_SIMD::AVX512>;

private:

    friend Base;

    template <bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _add(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                            const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr){
        if constexpr (IsScaled)
            (Memory::storeOffset<Is, Policy>(result, _mm512_add_epi32(_mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a), *scalarA),
                                                                      _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
        else
            (Memory::storeOffset<Is, Policy>(result, _mm512_add_epi32(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _axpy(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>, const T_simd *scalar = nullptr) {
        (Memory::storeOffset<Is, Policy>(result, _mm512_add_epi32(_mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a), *scalar),
                                                                                     Memory::loadOffset<Is, IsAligned>(b))), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                        const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr){
        if constexpr (IsScaled)
            (Memory::storeOffset<Is, Policy>(result, _mm512_sub_epi32(_mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a), *scalarA),
                                                                      _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
        else
            (Memory::storeOffset<Is, Policy>(result, _mm512_sub_epi32(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const T_data *a, const T_data *b, T_data *result, std::index_sequence<Is...>,
                                    const T_simd *scalarA = nullptr, const T_simd *scalarB = nullptr)
    {
        if constexpr (IsScaled)
            (Memory::storeOffset<Is, Policy>(result, _mm512_mullo_epi32(_mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a), *scalarA),
                                                                                      _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(b), *scalarB))),...);
        else
            (Memory::storeOffset<Is, Policy>(result, _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))),...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _scale(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(result, _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _scale(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(data, _mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(result, _mm512_add_epi32(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(data, _mm512_add_epi32(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }

    template <bool IsAligned, size_t... Is>
    static inline void _sum(const T_data* __restrict data, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_add_epi32(accumulators[Is], Memory::loadOffset<Is, IsAligned>(data))), ...);
    }

    template <bool IsAligned, size_t... Is>
    static inline void _dot(const T_data __restrict *a, const T_data __restrict *b, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_add_epi32(accumulators[Is],_mm512_mullo_epi32(Memory::loadOffset<Is, IsAligned>(a),
                                                                                  Memory::loadOffset<Is, IsAligned>(b)))), ...);
    }
};

template<>
struct VectorMathSIMD<short, T_SIMD::AVX512>
        : public VectorMathSIMDBase<short, T_SIMD::AVX512, VectorMathSIMD<short, T_SIMD::AVX512>> {
    
    using Base = VectorMathSIMDBase<short, T_SIMD::AVX512, VectorMathSIMD<short, T_SIMD::AVX512>>;
    using Traits = TypeTraitsSIMD<short, T_SIMD::AVX512>;
    using T_simd = typename Traits::typeSIMD;
    using T_data = typename Traits::typeData;
    using Memory = MemoryOperationsSIMD<short, T_SIMD::AVX512>;

private:
    
    friend Base;

    template<bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _add(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>, const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Memory::storeOffset<Is, Policy>(result,_mm512_add_epi16(_mm512_mullo_epi16(Memory::loadOffset<Is, IsAligned>(a), *scalarA),
                                                                     _mm512_mullo_epi16(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
        else
            (Memory::storeOffset<Is, Policy>(result, _mm512_add_epi16(Memory::loadOffset<Is, IsAligned>(a),
                                                            Memory::loadOffset<Is, IsAligned>(b))), ...);
    }

    template<bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _axpy(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>, const T_simd* scalar = nullptr) {
        (Memory::storeOffset<Is, Policy>(result, _mm512_add_epi16(_mm512_mullo_epi16(Memory::loadOffset<Is, IsAligned>(a), *scalar), 
                                                                                     Memory::loadOffset<Is, IsAligned>(b))), ...);
    }

    template<bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _subtract(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>, const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Memory::storeOffset<Is, Policy>(result, _mm512_sub_epi16(_mm512_mullo_epi16(Memory::loadOffset<Is, IsAligned>(a), *scalarA),
                                                                      _mm512_mullo_epi16(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
        else
            (Memory::storeOffset<Is, Policy>(result, _mm512_sub_epi16(Memory::loadOffset<Is, IsAligned>(a),
                                                            Memory::loadOffset<Is, IsAligned>(b))), ...);
    }

    template<bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _multiply(const T_data* a, const T_data* b, T_data* result, std::index_sequence<Is...>, const T_simd* scalarA = nullptr, const T_simd* scalarB = nullptr) {
        if constexpr (IsScaled)
            (Memory::storeOffset<Is, Policy>(
                result, _mm512_mullo_epi16(_mm512_mullo_epi16(Memory::loadOffset<Is, IsAligned>(a), *scalarA),
                                           _mm512_mullo_epi16(Memory::loadOffset<Is, IsAligned>(b), *scalarB))), ...);
        else
            (Memory::storeOffset<Is, Policy>(result, _mm512_mullo_epi16(Memory::loadOffset<Is, IsAligned>(a), Memory::loadOffset<Is, IsAligned>(b))), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, bool IsScaled, size_t... Is>
    static inline void _scale(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(result, _mm512_mullo_epi16(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _scale(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(data, _mm512_mullo_epi16(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(const T_data *data, T_data *result, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(result, _mm512_add_epi16(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }

    template <bool IsAligned, T_SIMDStore Policy, size_t... Is>
    static inline void _addConstant(__restrict T_data *data, const T_simd *scalar, std::index_sequence<Is...>) {
        (Memory::storeOffset<Is, Policy>(data, _mm512_add_epi16(Memory::loadOffset<Is, IsAligned>(data), *scalar)), ...);
    }

    template <bool IsAligned, size_t... Is>
    static inline void _sum(const T_data* __restrict data, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_add_epi16(accumulators[Is], Memory::loadOffset<Is, IsAligned>(data))), ...);
    }

    template <bool IsAligned, size_t... Is>
    static inline void _dot(const T_data __restrict *a, const T_data __restrict *b, T_simd* __restrict accumulators, std::index_sequence<Is...>) {
        ((accumulators[Is] = _mm512_add_epi16(accumulators[Is],_mm512_mullo_epi16(Memory::loadOffset<Is, IsAligned>(a),
                                                                                  Memory::loadOffset<Is, IsAligned>(b)))), ...);
    }
};



} // namespace Stalker::Mathematics

