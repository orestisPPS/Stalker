#ifndef MATH_OPERATIONS_H
#define MATH_OPERATIONS_H

#include <cmath>

namespace Stalker::Mathematics {

struct MathOperations {
public:
    template <typename T, typename ResultT = T>
    static inline void add(const T* a, const T * b, ResultT* result, unsigned size) {
        for (unsigned i = 0; i < size; ++i)
            result[i] = a[i] + b[i];
    }

    template <typename T, typename ResultT = T>
    static inline void add(const T* a, const T * b, ResultT* result, unsigned size, T scalarA, T scalarB) {
        for (unsigned i = 0; i < size; ++i)
            result[i] = (a[i] * scalarA) + (b[i] * scalarB);
    }

    template <typename T, typename ResultT = T>
    static inline void subtract(const T* a, const T * b, ResultT* result, unsigned size) {
        for (unsigned i = 0; i < size; ++i)
            result[i] = a[i] - b[i];
    }
    
    template <typename T, typename ResultT = T>
    static inline void subtract(const T* a, const T * b, ResultT* result, unsigned size, T scalarA, T scalarB) {
        for (unsigned i = 0; i < size; ++i)
            result[i] = (a[i] * scalarA) - (b[i] * scalarB);
    }

    template <typename T, typename ResultT = T>
    static inline void multiply(const T* a, const T * b, ResultT* result, unsigned size) {
        for (unsigned i = 0; i < size; ++i)
            result[i] = a[i] * b[i];
    }

    template <typename T, typename ResultT = T>
    static inline void multiply(const T* a, const T * b, ResultT* result, unsigned size, T scalarA, T scalarB) {
        for (unsigned i = 0; i < size; ++i)
            result[i] = (a[i] * scalarA) * (b[i] * scalarB);
    }

    template <typename T, typename ResultT = T>
    static inline void divide(const T* a, const T* b, ResultT* result, unsigned size) {
        for (unsigned i = 0; i < size; ++i)
            result[i] = a[i] / b[i];
    }

    template <typename T, typename ResultT = T>
    static inline void divide(const T* a, const T* b, ResultT* result, unsigned size, T scalarA, T scalarB) {
        for (unsigned i = 0; i < size; ++i)
            result[i] = (a[i] * scalarA) / (b[i] * scalarB);
    }

    template <typename T, typename ResultT = T>
    static inline void scale(const T* data, ResultT* result, unsigned size, T scalar) {
        for (unsigned i = 0; i < size; ++i)
            result[i] = data[i] * scalar;
    }

    template <typename T>
    static inline void scale(const T* __restrict data, unsigned size, T scalar) {
        for (unsigned i = 0; i < size; ++i)
            data[i] *= scalar;
    }

    template <typename T, typename ResultT = T>
    static inline ResultT dot(const T* a, const T* b, unsigned size) {
        ResultT result = ResultT{};
        for (unsigned i = 0; i < size; ++i)
            result += a[i] * b[i];
        return result;
    }

    template <typename T, typename ResultT = T>
    static inline ResultT dot(const T* a, const T* b, unsigned size, T scalarA, T scalarB) {
        ResultT result = ResultT{};
        for (unsigned i = 0; i < size; ++i)
            result += (a[i] * scalarA) * (b[i] * scalarB);
        return result;
    }

    template <typename T, typename ResultT = T>
    static inline ResultT cross2D(const T* a, const T* b) {
        return a[0] * b[1] - a[1] * b[0];
    }

    // 3D cross product returns a 3‐component vector
    template <typename T, typename ResultT = T>
    static inline void cross3D(const T* a, const T* b, ResultT* result) {
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
    }

    template <typename T, typename ResultT = T>
    static inline ResultT sum(const T* __restrict data, unsigned size) {
        ResultT result = ResultT{};
        for (unsigned i = 0; i < size; ++i)
            result += data[i];
        return result;
    }

    template <typename T, typename ResultT = T>
    static inline ResultT sumOfSquares(const T* __restrict data, unsigned size) {
        ResultT result = ResultT{};
        for (unsigned i = 0; i < size; ++i)
            result += data[i] * data[i];
        return result;
    }

    template <typename T, typename ResultT = T>
    static inline void normalize(const T* __restrict data, ResultT* __restrict result, unsigned size) {
        static_assert(std::is_floating_point<ResultT>::value, "Normalization result must be a floating-point type");
        ResultT norm = std::sqrt(sumOfSquares(data, size));
        if (norm > 0) {
            norm = 1 / std::sqrt(norm);
            for (unsigned i = 0; i < size; ++i)
                result[i] = data[i] * norm;
        }
    }

    template <typename T>
    static inline void normalize(const T* __restrict data, unsigned size) {
        static_assert(std::is_floating_point<T>::value, "In-place normalize requires a floating-point type");
        T norm = 1.0 / std::sqrt(sumOfSquares(data, size));
        for (unsigned i = 0; i < size; ++i)
            data[i] *= norm;
    }

    template <typename T>
    static inline T max(const T* __restrict data, unsigned size) {
        T max = data[0];
        for (unsigned i = 1; i < size; ++i) {
            if (data[i] > max)
                max = data[i];
        }
        return max;
    }

    template <typename T>
    static inline size_t maxIndex(const T* __restrict data, unsigned size) {
        T maxValue = data[0];
        size_t maxIdx = 0;
        for (unsigned i = 1; i < size; ++i) {
            if (data[i] > maxValue) {
                maxValue = data[i];
                maxIdx = i;
            }
        }
        return maxIdx;
    }

    template <typename T>
    static inline T min(const T* __restrict data, unsigned size) {
        T min = data[0];
        for (unsigned i = 1; i < size; ++i) {
            if (data[i] < min)
                min = data[i];
        }
        return min;
    }

    template <typename T>
    static inline size_t minIndex(const T* __restrict data, unsigned size) {
        T minValue = data[0];
        size_t minIdx = 0;
        for (unsigned i = 1; i < size; ++i) {
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
    static inline ResultT L1(const T* __restrict data, unsigned size) {
        static_assert(std::is_floating_point<ResultT>::value, "L1 norm result must be a floating-point type");
        T result = 0;
        for (unsigned i = 0; i < size; ++i)
            result += std::abs(data[i]);
        return result;
    }

    template <typename T, typename ResultT = T>
    static inline ResultT L2(const T* __restrict data, unsigned size) {
        static_assert(std::is_floating_point<ResultT>::value, "L2 norm result must be a floating-point type");
        return std::sqrt(MathOperations::sumOfSquares<T, ResultT>(data, size));
    }

    template <typename T, typename ResultT = T>
    static inline ResultT Linf(const T* __restrict data, unsigned size) {
        static_assert(std::is_floating_point<ResultT>::value, "Linf norm result must be a floating-point type");
        ResultT maxVal = std::abs(data[0]);
        for (unsigned i = 1; i < size; ++i)
            maxVal = std::max(maxVal, std::abs(data[i]));
        return maxVal;
    }

};

} // namespace Stalker::Mathematics
#endif // MATH_OPERATIONS_H