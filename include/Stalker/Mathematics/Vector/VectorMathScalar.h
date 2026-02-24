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

#include <cmath>
#include <cstddef>

namespace Stalker::Mathematics {

struct VectorMathScalar {
    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void add(size_t size, const T* a, const T * b, ResultT* result) {
        const T* end = a + size;
        for (; a != end; ++a, ++b, ++result)
            *result = (*a) + (*b);
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void add(size_t size, const T* a, const T * b, ResultT* result, T scalarA, T scalarB) {
        const T* end = a + size;
        for (; a != end; ++a, ++b, ++result)
            *result = ((*a) * scalarA) + ((*b) * scalarB);
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void axpy(size_t size, const T* a, const T * b, ResultT* result, T scalar) {
        const T* end = a + size;
        for (; a != end; ++a, ++b, ++result)
            *result = ((*a) * scalar) + (*b);
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void subtract(size_t size, const T* a, const T * b, ResultT* result) {
        const T* end = a + size;
        for (; a != end; ++a, ++b, ++result)
            *result = (*a) - (*b);
    }
    
    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void subtract(size_t size, const T* a, const T * b, ResultT* result, T scalarA, T scalarB) {
        const T* end = a + size;
        for (; a != end; ++a, ++b, ++result)
            *result = ((*a) * scalarA) - ((*b) * scalarB);
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void multiply(size_t size, const T* a, const T * b, ResultT* result) {
        const T* end = a + size;
        for (; a != end; ++a, ++b, ++result)
            *result = (*a) * (*b);
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void multiply(size_t size, const T* a, const T * b, ResultT* result, T scalarA, T scalarB) {
        const T* end = a + size;
        for (; a != end; ++a, ++b, ++result)
            *result = ((*a) * scalarA) * ((*b) * scalarB);
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void divide(size_t size, const T* a, const T* b, ResultT* result) {
        const T* end = a + size;
        for (; a != end; ++a, ++b, ++result)
            *result = (*a) / (*b);
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void divide(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
        const T* end = a + size;
        for (; a != end; ++a, ++b, ++result)
            *result = ((*a) * scalarA) / ((*b) * scalarB);
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void scale(size_t size, const T* data, ResultT* result, T scalar) {
        const T* end = data + size;
        for (; data != end; ++data, ++result)
            *result = (*data) * scalar;
    }

    template <typename T>
    STALKER_FORCE_INLINE constexpr static void scale(size_t size, T* STALKER_RESTRICT data, T scalar) {
        for (size_t i = 0; i < size; ++i)
            data[i] *= scalar;
    }

    template <typename T>
    STALKER_FORCE_INLINE constexpr static void addConstant(size_t size, const T* STALKER_RESTRICT data, T* STALKER_RESTRICT result, T constant) {
        const T* end = data + size;
        for (; data != end; ++data, ++result)
            *result = (*data) + constant;
    }

    template <typename T>
    STALKER_FORCE_INLINE constexpr static void addConstant(size_t size, STALKER_RESTRICT T* data, T constant) {
        for (size_t i = 0; i < size; ++i)
            data[i] += constant;
    }

    template <typename T>
    STALKER_FORCE_INLINE constexpr static T sum(size_t size, const T* STALKER_RESTRICT data) {
        T result = T{};
        const T* end = data + size;
        for (; data != end; ++data)
            result += *data;
        return result;
    }
    
    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static ResultT dot(size_t size, const T* a, const T* b) {
        ResultT result = ResultT{};
        const T* end = a + size;
        for (; a != end; ++a, ++b)
            result += (*a) * (*b);
        return result;
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static ResultT cross2D(size_t, const T* a, const T* b) {
        return a[0] * b[1] - a[1] * b[0];
    }

    // 3D cross product returns a 3‐component vector
    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void cross3D(size_t, const T* a, const T* b, ResultT* result) {
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static ResultT sumOfSquares(size_t size, const T* STALKER_RESTRICT data) {
        ResultT result = ResultT{};
        const T* end = data + size;
        for (; data != end; ++data)
            result += (*data) * (*data);
        return result;
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void normalize(size_t size, const T* STALKER_RESTRICT data, ResultT* STALKER_RESTRICT result) {
        static_assert(std::is_floating_point<ResultT>::value, "Normalization result must be a floating-point type");
        ResultT norm = std::sqrt(sumOfSquares(size, data));
        if (norm > 0) {
            norm = 1 / std::sqrt(norm);
            const T* end = data + size;
            for (; data != end; ++data, ++result)
                *result = (*data) * norm;
        }
    }

    template <typename T>
    STALKER_FORCE_INLINE constexpr static void normalize(size_t size, T* STALKER_RESTRICT data) {
        static_assert(std::is_floating_point<T>::value, "In-place normalize requires a floating-point type");
        T norm = 1.0 / std::sqrt(sumOfSquares(size, data));
        T* end = data + size;
        for (; data != end; ++data)
            *data *= norm;
    }

    template <typename T>
   STALKER_FORCE_INLINE constexpr static T max(size_t size, const T* STALKER_RESTRICT data) {
        T max = data[0];
        for (size_t i = 1; i < size; ++i) {
            if (data[i] > max)
                max = data[i];
        }
        return max;
    }

    template <typename T>
    STALKER_FORCE_INLINE constexpr static size_t maxIndex(size_t size, const T* STALKER_RESTRICT data) {
        T maxValue = data[0];
        size_t maxIdx = 0;
        for (size_t i = 1; i < size; ++i) {
            if (data[i] > maxValue) {
                maxValue = data[i];
                maxIdx = i;
            }
        }
        return maxIdx;
    }

    template <typename T>
    STALKER_FORCE_INLINE constexpr static T min(size_t size, const T* STALKER_RESTRICT data) {
        T min = data[0];
        for (size_t i = 1; i < size; ++i) {
            if (data[i] < min)
                min = data[i];
        }
        return min;
    }

    template <typename T>
    STALKER_FORCE_INLINE constexpr static size_t minIndex(size_t size, const T* STALKER_RESTRICT data) {
        T minValue = data[0];
        size_t minIdx = 0;
        for (size_t i = 1; i < size; ++i) {
            if (data[i] < minValue) {
                minValue = data[i];
                minIdx = i;
            }
        }
        return minIdx;
    }
};

struct Norms {
    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static ResultT L1(size_t size, const T* STALKER_RESTRICT data) {
        static_assert(std::is_floating_point<ResultT>::value, "L1 norm result must be a floating-point type");
        T result = 0;
        for (size_t i = 0; i < size; ++i)
            result += std::abs(data[i]);
        return result;
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static ResultT L2(size_t size, const T* STALKER_RESTRICT data) {
        static_assert(std::is_floating_point<ResultT>::value, "L2 norm result must be a floating-point type");
        return std::sqrt(VectorMathScalar::sumOfSquares<T, ResultT>(size, data));
    }
    
    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static ResultT Linf(size_t size, const T* STALKER_RESTRICT data) {
        static_assert(std::is_floating_point<ResultT>::value, "Linf norm result must be a floating-point type");
        ResultT maxVal = std::abs(data[0]);
        for (size_t i = 1; i < size; ++i)
            maxVal = std::max(maxVal, std::abs(data[i]));
        return maxVal;
    }
};

} // namespace Stalker::Mathematics