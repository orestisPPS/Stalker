#ifndef MATH_OPERATIONS_H
#define MATH_OPERATIONS_H

#include <cmath>
#include <cstddef>

namespace Stalker::Mathematics {

struct VectorMathScalar {
public:
    template <typename T, typename ResultT = T>
    static constexpr inline void add(size_t size, const T* a, const T * b, ResultT* result) {
        for (size_t i = 0; i < size; ++i)
            result[i] = a[i] + b[i];
    }

    template <typename T, typename ResultT = T>
    static constexpr inline void add(size_t size, const T* a, const T * b, ResultT* result, T scalarA, T scalarB) {
        for (size_t i = 0; i < size; ++i)
            result[i] = (a[i] * scalarA) + (b[i] * scalarB);
    }

    template <typename T, typename ResultT = T>
    static constexpr inline void axpy(size_t size, const T* a, const T * b, ResultT* result, T scalar) {
        for (size_t i = 0; i < size; ++i)
            result[i] = (a[i] * scalar) + b[i];
    }

    template <typename T, typename ResultT = T>
    static constexpr inline void subtract(size_t size, const T* a, const T * b, ResultT* result) {
        for (size_t i = 0; i < size; ++i)
            result[i] = a[i] - b[i];
    }
    
    template <typename T, typename ResultT = T>
    static constexpr inline void subtract(size_t size, const T* a, const T * b, ResultT* result, T scalarA, T scalarB) {
        for (size_t i = 0; i < size; ++i)
            result[i] = (a[i] * scalarA) - (b[i] * scalarB);
    }

    template <typename T, typename ResultT = T>
    static constexpr inline void multiply(size_t size, const T* a, const T * b, ResultT* result) {
        for (size_t i = 0; i < size; ++i)
            result[i] = a[i] * b[i];
    }

    template <typename T, typename ResultT = T>
    static constexpr inline void multiply(size_t size, const T* a, const T * b, ResultT* result, T scalarA, T scalarB) {
        for (size_t i = 0; i < size; ++i)
            result[i] = (a[i] * scalarA) * (b[i] * scalarB);
    }

    template <typename T, typename ResultT = T>
    static constexpr inline void divide(size_t size, const T* a, const T* b, ResultT* result) {
        for (size_t i = 0; i < size; ++i)
            result[i] = a[i] / b[i];
    }

    template <typename T, typename ResultT = T>
    static constexpr inline void divide(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
        for (size_t i = 0; i < size; ++i)
            result[i] = (a[i] * scalarA) / (b[i] * scalarB);
    }

    template <typename T, typename ResultT = T>
    static constexpr inline void scale(size_t size, const T* data, ResultT* result, T scalar) {
        for (size_t i = 0; i < size; ++i)
            result[i] = data[i] * scalar;
    }

    template <typename T>
    static constexpr inline void scale(size_t size, T* __restrict data, T scalar) {
        for (size_t i = 0; i < size; ++i)
            data[i] *= scalar;
    }

    template <typename T>
    static constexpr inline void addConstant(size_t size, const T* __restrict data, T* __restrict result, T constant) {
        for (size_t i = 0; i < size; ++i)
            result[i] = data[i] + constant;
    }

    template <typename T>
    static inline void addConstant(size_t size, __restrict T* data, T constant) {
        for (size_t i = 0; i < size; ++i)
            data[i] += constant;
    }

    template <typename T>
    static inline T sum(size_t size, const T* __restrict data) {
        T result = T{};
        for (size_t i = 0; i < size; ++i)
            result += data[i];
        return result;
    }
    
    template <typename T, typename ResultT = T>
    static inline ResultT dot(size_t size, const T* a, const T* b) {
        ResultT result = ResultT{};
        for (size_t i = 0; i < size; ++i)
            result += a[i] * b[i];
        return result;
    }

    template <typename T, typename ResultT = T>
    constexpr static inline ResultT cross2D(size_t, const T* a, const T* b) {
        return a[0] * b[1] - a[1] * b[0];
    }

    // 3D cross product returns a 3‐component vector
    template <typename T, typename ResultT = T>
    static constexpr inline void cross3D(size_t, const T* a, const T* b, ResultT* result) {
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
    }

    template <typename T, typename ResultT = T>
    constexpr static inline ResultT sumOfSquares(size_t size, const T* __restrict data) {
        ResultT result = ResultT{};
        for (size_t i = 0; i < size; ++i)
            result += data[i] * data[i];
        return result;
    }

    template <typename T, typename ResultT = T>
    static constexpr inline void normalize(size_t size, const T* __restrict data, ResultT* __restrict result) {
        static_assert(std::is_floating_point<ResultT>::value, "Normalization result must be a floating-point type");
        ResultT norm = std::sqrt(sumOfSquares(size, data));
        if (norm > 0) {
            norm = 1 / std::sqrt(norm);
            for (size_t i = 0; i < size; ++i)
                result[i] = data[i] * norm;
        }
    }

    template <typename T>
    static constexpr inline void normalize(size_t size, T* __restrict data) {
        static_assert(std::is_floating_point<T>::value, "In-place normalize requires a floating-point type");
        T norm = 1.0 / std::sqrt(sumOfSquares(size, data));
        for (size_t i = 0; i < size; ++i)
            data[i] *= norm;
    }

    template <typename T>
    static inline T max(size_t size, const T* __restrict data) {
        T max = data[0];
        for (size_t i = 1; i < size; ++i) {
            if (data[i] > max)
                max = data[i];
        }
        return max;
    }

    template <typename T>
    static inline size_t maxIndex(size_t size, const T* __restrict data) {
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
    static inline T min(size_t size, const T* __restrict data) {
        T min = data[0];
        for (size_t i = 1; i < size; ++i) {
            if (data[i] < min)
                min = data[i];
        }
        return min;
    }

    template <typename T>
    static inline size_t minIndex(size_t size, const T* __restrict data) {
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
    static inline ResultT L1(size_t size, const T* __restrict data) {
        static_assert(std::is_floating_point<ResultT>::value, "L1 norm result must be a floating-point type");
        T result = 0;
        for (size_t i = 0; i < size; ++i)
            result += std::abs(data[i]);
        return result;
    }

    template <typename T, typename ResultT = T>
    static inline ResultT L2(size_t size, const T* __restrict data) {
        static_assert(std::is_floating_point<ResultT>::value, "L2 norm result must be a floating-point type");
        return std::sqrt(VectorMathScalar::sumOfSquares<T, ResultT>(size, data));
    }

    template <typename T, typename ResultT = T>
    static inline ResultT Linf(size_t size, const T* __restrict data) {
        static_assert(std::is_floating_point<ResultT>::value, "Linf norm result must be a floating-point type");
        ResultT maxVal = std::abs(data[0]);
        for (size_t i = 1; i < size; ++i)
            maxVal = std::max(maxVal, std::abs(data[i]));
        return maxVal;
    }
};

} // namespace Stalker::Mathematics
#endif // MATH_OPERATIONS_H