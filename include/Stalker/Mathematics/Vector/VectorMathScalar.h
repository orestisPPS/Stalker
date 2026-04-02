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
#include <algorithm>
#include <numeric>

namespace Stalker::Mathematics {

struct VectorMathScalar {
    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void add(size_t size, const T* a, const T * b, ResultT* result) {
        if constexpr (UseSTD) {
            std::transform(a, a + size, b, result, [](const T& x, const T& y) { return static_cast<ResultT>(x + y); });
        } else {
            const T* end = a + size;
            for (; a != end; ++a, ++b, ++result)
                *result = (*a) + (*b);
        }
    }

    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void add(size_t size, const T* a, const T * b, ResultT* result, T scalarA, T scalarB) {
        if constexpr (UseSTD) {
            std::transform(a, a + size, b, result, [scalarA, scalarB](const T& x, const T& y) { return static_cast<ResultT>((x * scalarA) + (y * scalarB)); });
        } else {
            const T* end = a + size;
            for (; a != end; ++a, ++b, ++result)
                *result = ((*a) * scalarA) + ((*b) * scalarB);
        }
    }

    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void axpy(size_t size, const T* a, const T * b, ResultT* result, T scalar) {
        if constexpr (UseSTD) {
            std::transform(a, a + size, b, result, [scalar](const T& x, const T& y) { return static_cast<ResultT>((x * scalar) + y); });
        } else {
            const T* end = a + size;
            for (; a != end; ++a, ++b, ++result)
                *result = ((*a) * scalar) + (*b);
        }
    }

    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void subtract(size_t size, const T* a, const T * b, ResultT* result) {
        if constexpr (UseSTD) {
            std::transform(a, a + size, b, result, [](const T& x, const T& y) { return static_cast<ResultT>(x - y); });
        } else {
            const T* end = a + size;
            for (; a != end; ++a, ++b, ++result)
                *result = (*a) - (*b);
        }
    }
    
    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void subtract(size_t size, const T* a, const T * b, ResultT* result, T scalarA, T scalarB) {
        if constexpr (UseSTD) {
            std::transform(a, a + size, b, result, [scalarA, scalarB](const T& x, const T& y) { return static_cast<ResultT>((x * scalarA) - (y * scalarB)); });
        } else {
            const T* end = a + size;
            for (; a != end; ++a, ++b, ++result)
                *result = ((*a) * scalarA) - ((*b) * scalarB);
        }
    }

    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void multiply(size_t size, const T* a, const T * b, ResultT* result) {
        if constexpr (UseSTD) {
            std::transform(a, a + size, b, result, [](const T& x, const T& y) { return static_cast<ResultT>(x * y); });
        } else {
            const T* end = a + size;
            for (; a != end; ++a, ++b, ++result)
                *result = (*a) * (*b);
        }
    }

    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void multiply(size_t size, const T* a, const T * b, ResultT* result, T scalarA, T scalarB) {
        if constexpr (UseSTD) {
            std::transform(a, a + size, b, result, [scalarA, scalarB](const T& x, const T& y) { return static_cast<ResultT>((x * scalarA) * (y * scalarB)); });
        } else {
            const T* end = a + size;
            for (; a != end; ++a, ++b, ++result)
                *result = ((*a) * scalarA) * ((*b) * scalarB);
        }
    }

    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void divide(size_t size, const T* a, const T* b, ResultT* result) {
        if constexpr (UseSTD) {
            std::transform(a, a + size, b, result, [](const T& x, const T& y) { return static_cast<ResultT>(x / y); });
        } else {
            const T* end = a + size;
            for (; a != end; ++a, ++b, ++result)
                *result = (*a) / (*b);
        }
    }

    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void divide(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
        if constexpr (UseSTD) {
            std::transform(a, a + size, b, result, [scalarA, scalarB](const T& x, const T& y) { return static_cast<ResultT>((x * scalarA) / (y * scalarB)); });
        } else {
            const T* end = a + size;
            for (; a != end; ++a, ++b, ++result)
                *result = ((*a) * scalarA) / ((*b) * scalarB);
        }
    }

    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void scale(size_t size, const T* data, ResultT* result, T scalar) {
        if constexpr (UseSTD) {
            std::transform(data, data + size, result, [scalar](const T& x) { return static_cast<ResultT>(x * scalar); });
        } else {
            const T* end = data + size;
            for (; data != end; ++data, ++result)
                *result = (*data) * scalar;
        }
    }

    template <typename T, bool UseSTD = true>
    STALKER_FORCE_INLINE constexpr static void scale(size_t size, T* STALKER_RESTRICT data, T scalar) {
        if constexpr (UseSTD) {
            std::transform(data, data + size, data, [scalar](const T& x) { return x * scalar; });
        } else {
            for (size_t i = 0; i < size; ++i)
                data[i] *= scalar;
        }
    }

    template <typename T, bool UseSTD = true>
    STALKER_FORCE_INLINE constexpr static void addConstant(size_t size, const T* STALKER_RESTRICT data, T* STALKER_RESTRICT result, T constant) {
        if constexpr (UseSTD) {
            std::transform(data, data + size, result, [constant](const T& x) { return x + constant; });
        } else {
            const T* end = data + size;
            for (; data != end; ++data, ++result)
                *result = (*data) + constant;
        }
    }

    template <typename T, bool UseSTD = true>
    STALKER_FORCE_INLINE constexpr static void addConstant(size_t size, STALKER_RESTRICT T* data, T constant) {
        if constexpr (UseSTD) {
            std::transform(data, data + size, data, [constant](const T& x) { return x + constant; });
        } else {
            for (size_t i = 0; i < size; ++i)
                data[i] += constant;
        }
    }

    template <typename T, bool UseSTD = true>
    STALKER_FORCE_INLINE constexpr static T sum(size_t size, const T* STALKER_RESTRICT data) {
        if constexpr (UseSTD) {
            return std::accumulate(data, data + size, T{});
        } else {
            T result = T{};
            const T* end = data + size;
            for (; data != end; ++data)
                result += *data;
            return result;
        }
    }
    
    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static ResultT dot(size_t size, const T* a, const T* b) {
        if constexpr (UseSTD) {
            return std::inner_product(a, a + size, b, ResultT{});
        } else {
            ResultT result = ResultT{};
            const T* end = a + size;
            for (; a != end; ++a, ++b)
                result += (*a) * (*b);
            return result;
        }
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static ResultT cross2D(const T* a, const T* b) {
        return static_cast<ResultT>((a[0] * b[1]) - (a[1] * b[0]));
    }

    template <typename T, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static void cross3D(size_t, const T* a, const T* b, ResultT* result) {
        result[0] = static_cast<ResultT>(a[1] * b[2] - a[2] * b[1]);
        result[1] = static_cast<ResultT>(a[2] * b[0] - a[0] * b[2]);
        result[2] = static_cast<ResultT>(a[0] * b[1] - a[1] * b[0]);
    }

    template <typename T, bool UseSTD = true, typename ResultT = T>
    STALKER_FORCE_INLINE constexpr static ResultT sumOfSquares(size_t size, const T* STALKER_RESTRICT data) {
        if constexpr (UseSTD) {
            return std::inner_product(data, data + size, data, ResultT{});
        } else {
            ResultT result = ResultT{};
            const T* end = data + size;
            for (; data != end; ++data)
                result += (*data) * (*data);
            return result;
        }
    }

    template <typename T, bool UseSTD = true>
    STALKER_FORCE_INLINE constexpr static T max(size_t size, const T* STALKER_RESTRICT data) {
        if constexpr (UseSTD) {
            return *std::max_element(data, data + size);
        } else {
            T max = data[0];
            for (size_t i = 1; i < size; ++i) {
                if (data[i] > max)
                    max = data[i];
            }
            return max;
        }
    }

    template <typename T, bool UseSTD = true>
    STALKER_FORCE_INLINE constexpr static size_t maxIndex(size_t size, const T* STALKER_RESTRICT data) {
        if constexpr (UseSTD) {
            return std::distance(data, std::max_element(data, data + size));
        } else {
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
    }

    template <typename T, bool UseSTD = true>
    STALKER_FORCE_INLINE constexpr static T min(size_t size, const T* STALKER_RESTRICT data) {
        if constexpr (UseSTD) {
            return *std::min_element(data, data + size);
        } else {
            T min = data[0];
            for (size_t i = 1; i < size; ++i) {
                if (data[i] < min)
                    min = data[i];
            }
            return min;
        }
    }

    template <typename T, bool UseSTD = true>
    STALKER_FORCE_INLINE constexpr static size_t minIndex(size_t size, const T* STALKER_RESTRICT data) {
        if constexpr (UseSTD) {
            return std::distance(data, std::min_element(data, data + size));
        } else {
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
    }
};


namespace detail {
    template <typename T>
    STALKER_FORCE_INLINE constexpr static T absVal(T val) {
        if constexpr (std::is_unsigned_v<T>) {
            return val;
        } else {
            return val < 0 ? -val : val;
        }
    }
}

struct Norms {
    template <typename T, bool UseSTD = true>
    STALKER_FORCE_INLINE constexpr static double L1(size_t size, const T* STALKER_RESTRICT data) {
        if constexpr (UseSTD) {
            return std::accumulate(data, data + size, 0.0, [](double acc, T val) {
                return acc + detail::absVal(val);
            });
        } else {
            double result = 0;
            for (size_t i = 0; i < size; ++i)
                result += detail::absVal(data[i]);
            return result;
        }
    }

    template <typename T, bool UseSTD = true>
    STALKER_FORCE_INLINE constexpr static double L2(size_t size, const T* STALKER_RESTRICT data) {
        return std::sqrt(VectorMathScalar::sumOfSquares<T, UseSTD, double>(size, data));
    }
    
    template <typename T, bool UseSTD = true>
    STALKER_FORCE_INLINE constexpr static double Linf(size_t size, const T* STALKER_RESTRICT data) {
        if constexpr (UseSTD) {
            return detail::absVal(*std::max_element(data, data + size, [](T a, T b){
                return detail::absVal(a) < detail::absVal(b);
            }));
        } else {
            double maxVal = detail::absVal(data[0]);
            for (size_t i = 1; i < size; ++i)
                maxVal = std::max(maxVal, detail::absVal(data[i]));
            return maxVal;
        }
    }
};

} // namespace Stalker::Mathematics
