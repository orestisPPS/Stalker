#ifndef META_MATH_OPERATIONS_H
#define META_MATH_OPERATIONS_H
#include <cstddef>

namespace Stalker::Mathematics {

struct MathOperationsMeta {
public:

    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline void add(const T* a, const T* b, ResultT* result, size_t size) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _add<T, ResultT, false>(a + i, b + i, result + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; ++i)
            result[i] = a[i] + b[i];
    }

    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline void add(const T* a, const T* b, ResultT* result, size_t size, T scalarA, T scalarB) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _add<T, ResultT, true>(a + i, b + i, result + i, std::make_index_sequence<UnrollFactor>{}, scalarA, scalarB);
        for (size_t i = limit; i < size; ++i)
            result[i] = (a[i] * scalarA) + (b[i] * scalarB);
    }

    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline void subtract(const T* a, const T* b, ResultT* result, size_t size) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _subtract<T, ResultT, false>(a + i, b + i, result + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; ++i)
            result[i] = a[i] - b[i];
    }

    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline void subtract(const T* a, const T* b, ResultT* result, size_t size, T scalarA, T scalarB) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _subtract<T, ResultT, true>(a + i, b + i, result + i, std::make_index_sequence<UnrollFactor>{}, scalarA, scalarB);
        for (size_t i = limit; i < size; ++i)
            result[i] = (a[i] * scalarA) - (b[i] * scalarB);
    }

    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline void multiply(const T* a, const T* b, ResultT* result, size_t size) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _multiply<T, ResultT, false>(a + i, b + i, result + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; ++i)
            result[i] = a[i] * b[i];
    }

    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline void multiply(const T* a, const T* b, ResultT* result, size_t size, T scalarA, T scalarB) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _multiply<T, ResultT, true>(a + i, b + i, result + i, std::make_index_sequence<UnrollFactor>{}, scalarA, scalarB);
        for (size_t i = limit; i < size; ++i)
            result[i] = (a[i] * scalarA) * (b[i] * scalarB);
    }
    
    // Divide
    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline void divide(const T* a, const T* b, ResultT* result, size_t size) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _divide<T, ResultT, false>(a + i, b + i, result + i, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; ++i)
            result[i] = a[i] / b[i];
    }

    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline void divide(const T* a, const T* b, ResultT* result, size_t size, T scalarA, T scalarB) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _divide<T, ResultT, true>(a + i, b + i, result + i, std::make_index_sequence<UnrollFactor>{}, scalarA, scalarB);
        for (size_t i = limit; i < size; ++i)
            result[i] = (a[i] * scalarA) / (b[i] * scalarB);
    }
    // Scale
    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline void scale(const T* __restrict data, ResultT* __restrict result, size_t size, T scalar) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _scale<T, ResultT>(data + i, result + i, scalar, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; ++i)
            result[i] = data[i] * scalar;
    }

    template <typename T, size_t UnrollFactor>
    static constexpr inline void scale(const T* __restrict  data, size_t size, T scalar) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _scale<T, T>(data + i, scalar, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; ++i)
            data[i] *= scalar;
    }

    // Add Constant
    template <typename T, size_t Size, typename ResultT = T>
    static constexpr inline void addConstant(const T* __restrict data, ResultT* __restrict result, T constant) {
        _addConstant<T, ResultT,  Size>(data, result, constant, std::make_index_sequence<Size>{});
    }
    template <typename T, size_t Size>
    static constexpr inline void addConstant(const T* __restrict data, T constant) {
        _addConstant<T, T, Size>(data, constant, std::make_index_sequence<Size>{});
    }
    //needs test
    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline void addConstant(const T* __restrict data, T* __restrict result, size_t size, T constant) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _addConstant<T, ResultT>(data + i, result + i, constant, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; ++i)
            result[i] = data[i] + constant;
    }
    //needs test
    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline void addConstant(const T* __restrict data, size_t size, T constant) {
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _addConstant<T, ResultT>(data + i, constant, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; ++i)
            data[i] += constant;
    }

    // Sum
    template <typename T, size_t Size>
    static constexpr inline T sum(const T* __restrict data) {
        T result = T{};
        _sum<T, T>(data, result, std::make_index_sequence<Size>{});
        return result;
    }
    template <typename T, size_t Size>
    static constexpr inline T sum(const T* __restrict data, size_t size) {
        T result = 0;
        auto limit = size - (size % Size);
        for (size_t i = 0; i < limit; i += Size)
            _sum<T, T>(data + i, result, std::make_index_sequence<Size>{});
        for (size_t i = limit; i < size; ++i)
            result += data[i];
        return result;
    }
    // Dot Product
    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline T dot(const T* a, const T* b, size_t size) {
        T result = 0;
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _dot<T, T, false>(a + i, b + i, result, std::make_index_sequence<UnrollFactor>{});
        for (size_t i = limit; i < size; ++i)
            result += a[i] * b[i];
        return result;
    }

    template <typename T, size_t UnrollFactor, typename ResultT = T>
    static constexpr inline T dot(const T* a, const T* b, size_t size, T scalarA, T scalarB) {
        T result = 0;
        auto limit = size - (size % UnrollFactor);
        for (size_t i = 0; i < limit; i += UnrollFactor)
            _dot<T, ResultT, true>(a + i, b + i, result, std::make_index_sequence<UnrollFactor>{}, scalarA, scalarB);
        for (size_t i = limit; i < size; ++i)
            result += (a[i] * scalarA) * (b[i] * scalarB);
        return result;
    }

private:
    template <typename T, typename R, bool isScaled, size_t... Indices>
    static constexpr inline void _add(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
        if constexpr (isScaled) {
            ((result[Indices] = (a[Indices] * scalarA) + (b[Indices] * scalarB)), ...);
        } else {
            ((result[Indices] = a[Indices] + b[Indices]), ...);
        }
    }

    template <typename T, typename R, bool isScaled, size_t... Indices>
    static constexpr inline void _subtract(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
        if constexpr (isScaled) {
            ((result[Indices] = (a[Indices] * scalarA) - (b[Indices] * scalarB)), ...);
        } else {
            ((result[Indices] = a[Indices] - b[Indices]), ...);
        }
    }

    template <typename T, typename R, bool isScaled, size_t... Indices>
    static constexpr inline void _multiply(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
        if constexpr (isScaled) {
            ((result[Indices] = (a[Indices] * scalarA) * (b[Indices] * scalarB)), ...);
        } else {
            ((result[Indices] = a[Indices] * b[Indices]), ...);
        }
    }

    template <typename T, typename R, bool isScaled, size_t... Indices>
    static constexpr inline void _divide(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
        if constexpr (isScaled) {
            ((result[Indices] = (a[Indices] * scalarA) / (b[Indices] * scalarB)), ...);
        } else {
            ((result[Indices] = a[Indices] / b[Indices]), ...);
        }
    }

    template <typename T, typename R, size_t... Indices>
    static constexpr inline void _scale(const T* __restrict data, T* __restrict result, T scalar, std::index_sequence<Indices...>) {
        ((result[Indices] = data[Indices] * scalar), ...);
    }

    template <typename T, typename R, size_t... Indices>
    static constexpr inline void _scale(const T* __restrict data, T scalar, std::index_sequence<Indices...>) {
        ((data[Indices] *= scalar), ...);
    }

    template <typename T, typename R, size_t... Indices>
    static constexpr inline void _addConstant(const __restrict T* data, T*  __restrict result, T constant, std::index_sequence<Indices...>) {
        ((result[Indices] = data[Indices] + constant), ...);
    }

    template <typename T, typename R, size_t... Indices>
    static constexpr inline void _addConstant(const __restrict T* data, T constant, std::index_sequence<Indices...>) {
        ((data[Indices] += constant), ...);
    }

    template <typename T, typename R, size_t... Indices>
    static constexpr inline void _sum (const T* __restrict data, T &result, std::index_sequence<Indices...>) {
        ((result += data[Indices]), ...);
    }

    template <typename T, typename R, bool isScaled, size_t... Indices>
    static constexpr inline void _dot (const T* a, const T* b, T &result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
        if constexpr (isScaled)
            ((result += (a[Indices] * scalarA) * (b[Indices] * scalarB)), ...);
        else
            ((result += a[Indices] * b[Indices]), ...);
    }





    /**
     * @brief Applies a stateless unary operation to each element of a contiguous data array and returns the results in a fixed-size std::array.
     *
     * @tparam T     Element type of the input data and output array.
     * @tparam Size  Number of elements to process (compile-time constant).
     * @tparam OpT   Type of the callable object invoked for each element.
     *
     * @param data       Pointer to the first element of the input array (must point to at least Size elements).
     * @param operation  Callable that accepts a T value and returns a transformed T value. Must be invocable as operation(data[i]).
     *
     * @return A std::array<T, Size> whose elements are operation(data[0]), operation(data[1]), …, operation(data[Size-1]).
     *
     * @note Guaranteed constexpr and inline: the lambda and fold are unrolled at compile time, producing zero-overhead code.
     */
    template <typename T, size_t Size, typename OpT>
    [[nodiscard]] static constexpr inline auto
    _unaryOperation(const T* __restrict data, OpT&& operation) {
        return [&]<size_t... Indices>(std::index_sequence<Indices...>) {
            return std::array<T, Size>{{ operation(data[Indices])... }};
        }(std::make_index_sequence<Size>{});
    }

    /**
     * @brief Applies a stateless unary operation to each element of a source array and writes the results into a destination buffer.
     *
     * @tparam T     Element type of the input and output buffers.
     * @tparam Size  Number of elements to process (compile-time constant).
     * @tparam OpT   Type of the callable object invoked for each element.
     *
     * @param source       Pointer to the first element of the input array (must point to at least Size elements).
     * @param destination  Pointer to the first element of the output array (must point to at least Size elements).
     * @param operation    Callable that accepts a T value and returns a transformed T value. Must be invocable as operation(source[i]).
     *
     * @note This overload avoids creating a temporary std::array by writing results directly into destination.
     * @note constexpr and inline: fully unrolled at compile time for predictable performance.
     */
    template <typename T, size_t Size, typename OpT>
    static constexpr inline void
    _unaryOperation(const T* source, T* destination, OpT&& operation) {
        [&]<size_t... Indices>(std::index_sequence<Indices...>) {
            ((destination[Indices] = operation(source[Indices])), ...);
        }(std::make_index_sequence<Size>{});
    }

    template<typename T, size_t Size, typename OpT>
    static constexpr inline void
    _unaryOperationReduced(const T* __restrict source, T& result, OpT&& operation) {
        [&]<size_t... Indices>(std::index_sequence<Indices...>) {
            // Fold over comma: executes result=op(result,source[Indices]) for each Indices
            ((result = operation(result, source[Indices])), ...);
        }(std::make_index_sequence<Size>{});  // generate 0…Size-1
    }

    /**
     * @brief Applies a stateless binary operation to corresponding elements of two input arrays and returns the results in a fixed-size std::array.
     *
     * @tparam T     Element type of the input and output arrays.
     * @tparam Size  Number of element pairs to process (compile-time constant).
     * @tparam OpT   Type of the callable object invoked for each pair.
     *
     * @param a          Pointer to the first element of the first input array (must point to at least Size elements).
     * @param b          Pointer to the first element of the second input array (must point to at least Size elements).
     * @param operation  Callable that accepts two T values and returns a computed T value. Must be invocable as operation(a[i], b[i]).
     *
     * @return A std::array<T, Size> whose elements are operation(a[0], b[0]), operation(a[1], b[1]), …, operation(a[Size-1], b[Size-1]).
     *
     * @note constexpr and inline: the generic lambda and fold produce unrolled loops at compile time, yielding zero-overhead.
     */
    template <typename T, size_t Size, typename OpT>
    [[nodiscard]] static constexpr inline  auto _binaryOperation(const T* a, const T* b, OpT&& operation) {
        return [&]<size_t... Indices>(std::index_sequence<Indices...>) {
            return std::array<T, Size>{{ operation(a[Indices], b[Indices])... }};
        }(std::make_index_sequence<Size>{});
    }

    /**
     * @brief Applies a stateless binary operation to corresponding elements of two input arrays and writes the results into a destination buffer.
     *
     * @tparam T     Element type of the input and output buffers.
     * @tparam Size  Number of element pairs to process (compile-time constant).
     * @tparam OpT   Type of the callable object invoked for each pair.
     *
     * @param a           Pointer to the first element of the first input array (must point to at least Size elements).
     * @param b           Pointer to the first element of the second input array (must point to at least Size elements).
     * @param destination Pointer to the first element of the output array (must point to at least Size elements).
     * @param operation   Callable that accepts two T values and returns a computed T value. Must be invocable as operation(a[i], b[i]).
     *
     * @note This overload avoids creating a temporary std::array by writing results directly into destination.
     * @note constexpr and inline: fully unrolled at compile time for consistent real-time performance.
     */
    template <typename T, size_t Size, typename OpT>
    static constexpr inline void _binaryOperation(const T* a, const T* b, T* destination, OpT&& operation) {
        [&]<size_t... Indices>(std::index_sequence<Indices...>) {
            ((destination[Indices] = operation(a[Indices], b[Indices])), ...);
        }(std::make_index_sequence<Size>{});
    }

    template<typename T, size_t Size, typename OpT>
    static constexpr inline void
    _binaryOperationReduced(const T* a, const T* b, T& result, OpT&& operation) {
        [&]<size_t... Idx>(std::index_sequence<Idx...>) {
            ((result = operation(result, a[Idx], b[Idx])), ...);
        }(std::make_index_sequence<Size>{});
    }

};

} // namespace Stalker::Mathematics
#endif // META_MATH_OPERATIONS_H