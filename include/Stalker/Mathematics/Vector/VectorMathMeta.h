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

#include <cstddef>
#include <Stalker/Core/Config/LoopUnrolling.h>

namespace Stalker::Mathematics {
    
using namespace Stalker::Core::Config;

    struct VectorMathMeta {
    public:

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void add(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _add<T, ResultT, false>(a, b, result, UnrollIndexSequence<Unroll>{});
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = (*a) + (*b);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void add(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _add<T, ResultT, true>(a, b, result, UnrollIndexSequence<Unroll>{}, scalarA, scalarB);
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = ((*a) * scalarA) + ((*b) * scalarB);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void axpy(size_t size, const T* a, const T* b, ResultT* result, T scalarA) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _add<T, ResultT, true>(a, b, result, UnrollIndexSequence<Unroll>{}, scalarA);
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = ((*a) * scalarA) + (*b);
        }



        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void subtract(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _subtract<T, ResultT, false>(a, b, result, UnrollIndexSequence<Unroll>{});
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = (*a) - (*b);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void subtract(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _subtract<T, ResultT, true>(a, b, result, UnrollIndexSequence<Unroll>{}, scalarA, scalarB);
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = ((*a) * scalarA) - ((*b) * scalarB);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void multiply(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _mul<T, ResultT, false>(a, b, result, UnrollIndexSequence<Unroll>{});
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = (*a) * (*b);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void multiply(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _mul<T, ResultT, true>(a, b, result, UnrollIndexSequence<Unroll>{}, scalarA, scalarB);
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = ((*a) * scalarA) * ((*b) * scalarB);
        }
        
        // Divide
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void divide(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _div<T, ResultT, false>(a, b, result, UnrollIndexSequence<Unroll>{});
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = (*a) / (*b);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void divide(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _div<T, ResultT, true>(a, b, result, UnrollIndexSequence<Unroll>{}, scalarA, scalarB);
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = ((*a) * scalarA) / ((*b) * scalarB);
        }
        // Scale
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void scale(size_t size, const T* STALKER_RESTRICT data, ResultT* STALKER_RESTRICT result, T scalar) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = data + size;
            const T* end = data + limit;
            for (; data != end; data += Unroll, result += Unroll)
                _scale<T, ResultT>(data, result, scalar, UnrollIndexSequence<Unroll>{});
            for (; data != totalEnd; ++data, ++result)
                *result = (*data) * scalar;
        }

        template <typename T, size_t Unroll = DefaultUnroll()>
        STALKER_FORCE_INLINE constexpr static void scale(size_t size, T* STALKER_RESTRICT data, T scalar) {
            auto limit = size - (size % Unroll);
            T* totalEnd = data + size;
            T* end = data + limit;
            for (; data != end; data += Unroll)
                _scale<T, T>(data, scalar, UnrollIndexSequence<Unroll>{});
            for (; data != totalEnd; ++data)
                *data *= scalar;
        }

        //needs test
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void addConstant(size_t size, const T* STALKER_RESTRICT data, T* STALKER_RESTRICT result, T constant) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = data + size;
            const T* end = data + limit;
            for (; data != end; data += Unroll, result += Unroll)
                _addConstant<T, ResultT>(data, result, constant, UnrollIndexSequence<Unroll>{});
            for (; data != totalEnd; ++data, ++result)
                *result = (*data) + constant;
        }
        //needs test
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static void addConstant(size_t size, T* STALKER_RESTRICT data, T constant) {
            auto limit = size - (size % Unroll);
            T* totalEnd = data + size;
            T* end = data + limit;
            for (; data != end; data += Unroll)
                _addConstant<T, ResultT>(data, constant, UnrollIndexSequence<Unroll>{});
            for (; data != totalEnd; ++data)
                *data += constant;
        }

        // Sum
        template <typename T, size_t Unroll = DefaultUnroll()>
        STALKER_FORCE_INLINE constexpr static T sum(size_t size, const T* STALKER_RESTRICT data) {
            T result = 0;
            auto limit = size - (size % Unroll);
            const T* totalEnd = data + size;
            const T* end = data + limit;
            for (; data != end; data += Unroll)
                _sum<T, T>(data, result, UnrollIndexSequence<Unroll>{});
            for (; data != totalEnd; ++data)
                result += *data;
            return result;
        }
        // Dot Product
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        STALKER_FORCE_INLINE constexpr static T dot(size_t size, const T* a, const T* b) {
            T result = 0;
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll)
                _dot<T, ResultT>(a, b, result, UnrollIndexSequence<Unroll>{});
            for (; a != totalEnd; ++a, ++b)
                result += (*a) * (*b);
            return result;
        }

    private:
        template <typename T, typename R, bool IsScaled, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _add(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
            if constexpr (IsScaled) {
                ((result[Indices] = (a[Indices] * scalarA) + (b[Indices] * scalarB)), ...);
            } else {
                ((result[Indices] = a[Indices] + b[Indices]), ...);
            }
        }

        template <typename T, typename R, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _axpy(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalar) {
            ((result[Indices] = (a[Indices] * scalar) + b[Indices]), ...);
        }

        template <typename T, typename R, bool IsScaled, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _subtract(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
            if constexpr (IsScaled) {
                ((result[Indices] = (a[Indices] * scalarA) - (b[Indices] * scalarB)), ...);
            } else {
                ((result[Indices] = a[Indices] - b[Indices]), ...);
            }
        }

        template <typename T, typename R, bool IsScaled, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _mul(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
            if constexpr (IsScaled) {
                ((result[Indices] = (a[Indices] * scalarA) * (b[Indices] * scalarB)), ...);
            } else {
                ((result[Indices] = a[Indices] * b[Indices]), ...);
            }
        }

        template <typename T, typename R, bool IsScaled, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _div(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
            if constexpr (IsScaled) {
                ((result[Indices] = (a[Indices] * scalarA) / (b[Indices] * scalarB)), ...);
            } else {
                ((result[Indices] = a[Indices] / b[Indices]), ...);
            }
        }

        template <typename T, typename R, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _scale(const T* STALKER_RESTRICT data, T* STALKER_RESTRICT result, T scalar, std::index_sequence<Indices...>) {
            ((result[Indices] = data[Indices] * scalar), ...);
        }

        template <typename T, typename R, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _scale(T* STALKER_RESTRICT data, T scalar, std::index_sequence<Indices...>) {
            ((data[Indices] *= scalar), ...);
        }

        template <typename T, typename R, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _addConstant(const STALKER_RESTRICT T* data, T*  STALKER_RESTRICT result, T constant, std::index_sequence<Indices...>) {
            ((result[Indices] = data[Indices] + constant), ...);
        }

        template <typename T, typename R, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _addConstant(STALKER_RESTRICT T* data, T constant, std::index_sequence<Indices...>) {
            ((data[Indices] += constant), ...);
        }

        template <typename T, typename R, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _sum (const T* STALKER_RESTRICT data, T &result, std::index_sequence<Indices...>) {
            ((result += data[Indices]), ...);
        }

        template <typename T, typename R, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _dot (const T* a, const T* b, T &result, std::index_sequence<Indices...>) {
            ((result += a[Indices] * b[Indices]), ...);
        }
    };

} // namespace Stalker::Mathematics