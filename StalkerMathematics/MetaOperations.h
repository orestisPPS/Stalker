#ifndef META_OPERATIONS_H
#define META_OPERATIONS_H
#include <cstddef>

struct MetaOperations {
public:  
template <typename T, size_t Size>
    static constexpr inline void add(const T* data1, const T* data2, T* result, T scale1, T scale2) {
        if constexpr (Size > 0) {
            result[Size - 1] = data1[Size - 1] * scale1 + data2[Size - 1] * scale2;
            add<T, Size - 1>(data1, data2, result, scale1, scale2);
        }
    }

    template <typename T, size_t Size>
    static constexpr inline void subtract(const T* data1, const T* data2, T* result, T scale1, T scale2) {
        add<T, Size>(data1, data2, result, scale1, -scale2);
    }

    template <typename T, size_t Size>
    static constexpr inline void multiply(const T* data1, const T* data2, T* result, T scale1, T scale2) {
        if constexpr (Size > 0) {
            result[Size - 1] = data1[Size - 1] * scale1 * data2[Size - 1] * scale2;
            multiply<T, Size - 1>(data1, data2, result, scale1, scale2);
        }
    }

    template <typename T, size_t Size>
    static constexpr inline void divide(const T* data1, const T* data2, T* result, T scale1, T scale2) {
        multiply<T, Size>(data1, data2, result, scale1, 1 / scale2);
    }

    template <typename T, size_t Size>
    static constexpr inline void negate(const T* data, T* result) {
        _negate(data, result, std::make_index_sequence<Size>{});
    }

    template <typename T, size_t Size>
    static constexpr inline void scale(const T* data, T* result, T scale) {
        return _scale(data, result, scale, std::make_index_sequence<Size>{});
    }

    template <typename T, size_t Size>
    static constexpr inline T dotProduct(const T* data1, const T* data2) {
        return _dotProduct(data1, data2, std::make_index_sequence<Size>{});
    }

    template <typename T, size_t Size>
    static constexpr inline T sum(const T* data) {
        return _sum(data, std::make_index_sequence<Size>{});
    }

    template <typename T, size_t Size>
    static constexpr inline T mean(const T* data) {
        return sum<T, Size>(data) / Size;
    }

    private:

        template <typename T, size_t... IndexSequence>
        static constexpr inline T _dotProduct(const T* data1, const T* data2, std::index_sequence<IndexSequence...>) {
            return ((data1[IndexSequence] * data2[IndexSequence]) + ...);
        }

        template <typename T, size_t... IndexSequence>
        static constexpr inline T _sum(const T* data, std::index_sequence<IndexSequence...>) {
            return ((data[IndexSequence] + ...));
        }

        template <typename T, size_t... IndexSequence>
        static constexpr inline T _scale(const T* data, T scale, std::index_sequence<IndexSequence...>) {
            return ((data[IndexSequence] * scale) + ...);
        }

        template <typename T, size_t... IndexSequence>
        static constexpr inline void _negate(const T* data, T* result, std::index_sequence<IndexSequence...>) {
            ((result[IndexSequence] = -data[IndexSequence]), ...);
        }





};

#endif // META_OPERATIONS_H