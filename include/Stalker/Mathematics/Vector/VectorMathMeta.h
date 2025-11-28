#pragma once

#include <cstddef>
#include <Stalker/Core/Config/LoopUnrolling.h>

namespace Stalker::Mathematics {
    
using namespace Stalker::Core::Config;

    struct VectorMathMeta {
    public:

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void add(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _add<T, ResultT, false>(a, b, result, std::make_index_sequence<Unroll>{});
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = (*a) + (*b);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void add(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _add<T, ResultT, true>(a, b, result, std::make_index_sequence<Unroll>{}, scalarA, scalarB);
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = ((*a) * scalarA) + ((*b) * scalarB);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void axpy(size_t size, const T* a, const T* b, ResultT* result, T scalarA) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _add<T, ResultT, true>(a, b, result, std::make_index_sequence<Unroll>{}, scalarA);
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = ((*a) * scalarA) + (*b);
        }



        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void subtract(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _subtract<T, ResultT, false>(a, b, result, std::make_index_sequence<Unroll>{});
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = (*a) - (*b);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void subtract(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _subtract<T, ResultT, true>(a, b, result, std::make_index_sequence<Unroll>{}, scalarA, scalarB);
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = ((*a) * scalarA) - ((*b) * scalarB);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void multiply(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _multiply<T, ResultT, false>(a, b, result, std::make_index_sequence<Unroll>{});
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = (*a) * (*b);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void multiply(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _multiply<T, ResultT, true>(a, b, result, std::make_index_sequence<Unroll>{}, scalarA, scalarB);
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = ((*a) * scalarA) * ((*b) * scalarB);
        }
        
        // Divide
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void divide(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _divide<T, ResultT, false>(a, b, result, std::make_index_sequence<Unroll>{});
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = (*a) / (*b);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void divide(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll, result += Unroll)
                _divide<T, ResultT, true>(a, b, result, std::make_index_sequence<Unroll>{}, scalarA, scalarB);
            for (; a != totalEnd; ++a, ++b, ++result)
                *result = ((*a) * scalarA) / ((*b) * scalarB);
        }
        // Scale
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void scale(size_t size, const T* __restrict data, ResultT* __restrict result, T scalar) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = data + size;
            const T* end = data + limit;
            for (; data != end; data += Unroll, result += Unroll)
                _scale<T, ResultT>(data, result, scalar, std::make_index_sequence<Unroll>{});
            for (; data != totalEnd; ++data, ++result)
                *result = (*data) * scalar;
        }

        template <typename T, size_t Unroll = DefaultUnroll()>
        static constexpr inline void scale(size_t size, T* __restrict data, T scalar) {
            auto limit = size - (size % Unroll);
            T* totalEnd = data + size;
            T* end = data + limit;
            for (; data != end; data += Unroll)
                _scale<T, T>(data, scalar, std::make_index_sequence<Unroll>{});
            for (; data != totalEnd; ++data)
                *data *= scalar;
        }

        //needs test
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void addConstant(size_t size, const T* __restrict data, T* __restrict result, T constant) {
            auto limit = size - (size % Unroll);
            const T* totalEnd = data + size;
            const T* end = data + limit;
            for (; data != end; data += Unroll, result += Unroll)
                _addConstant<T, ResultT>(data, result, constant, std::make_index_sequence<Unroll>{});
            for (; data != totalEnd; ++data, ++result)
                *result = (*data) + constant;
        }
        //needs test
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void addConstant(size_t size, T* __restrict data, T constant) {
            auto limit = size - (size % Unroll);
            T* totalEnd = data + size;
            T* end = data + limit;
            for (; data != end; data += Unroll)
                _addConstant<T, ResultT>(data, constant, std::make_index_sequence<Unroll>{});
            for (; data != totalEnd; ++data)
                *data += constant;
        }

        // Sum
        template <typename T, size_t Unroll = DefaultUnroll()>
        static constexpr inline T sum(size_t size, const T* __restrict data) {
            T result = 0;
            auto limit = size - (size % Unroll);
            const T* totalEnd = data + size;
            const T* end = data + limit;
            for (; data != end; data += Unroll)
                _sum<T, T>(data, result, std::make_index_sequence<Unroll>{});
            for (; data != totalEnd; ++data)
                result += *data;
            return result;
        }
        // Dot Product
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline T dot(size_t size, const T* a, const T* b) {
            T result = 0;
            auto limit = size - (size % Unroll);
            const T* totalEnd = a + size;
            const T* end = a + limit;
            for (; a != end; a += Unroll, b += Unroll)
                _dot<T, ResultT>(a, b, result, std::make_index_sequence<Unroll>{});
            for (; a != totalEnd; ++a, ++b)
                result += (*a) * (*b);
            return result;
        }

    private:
        template <typename T, typename R, bool IsScaled, size_t... Indices>
        static constexpr inline void _add(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
            if constexpr (IsScaled) {
                ((result[Indices] = (a[Indices] * scalarA) + (b[Indices] * scalarB)), ...);
            } else {
                ((result[Indices] = a[Indices] + b[Indices]), ...);
            }
        }

        template <typename T, typename R, size_t... Indices>
        static constexpr inline void _axpy(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalar) {
            ((result[Indices] = (a[Indices] * scalar) + b[Indices]), ...);
        }

        template <typename T, typename R, bool IsScaled, size_t... Indices>
        static constexpr inline void _subtract(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
            if constexpr (IsScaled) {
                ((result[Indices] = (a[Indices] * scalarA) - (b[Indices] * scalarB)), ...);
            } else {
                ((result[Indices] = a[Indices] - b[Indices]), ...);
            }
        }

        template <typename T, typename R, bool IsScaled, size_t... Indices>
        static constexpr inline void _multiply(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
            if constexpr (IsScaled) {
                ((result[Indices] = (a[Indices] * scalarA) * (b[Indices] * scalarB)), ...);
            } else {
                ((result[Indices] = a[Indices] * b[Indices]), ...);
            }
        }

        template <typename T, typename R, bool IsScaled, size_t... Indices>
        static constexpr inline void _divide(const T* a, const T* b, T* result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
            if constexpr (IsScaled) {
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
        static constexpr inline void _scale(T* __restrict data, T scalar, std::index_sequence<Indices...>) {
            ((data[Indices] *= scalar), ...);
        }

        template <typename T, typename R, size_t... Indices>
        static constexpr inline void _addConstant(const __restrict T* data, T*  __restrict result, T constant, std::index_sequence<Indices...>) {
            ((result[Indices] = data[Indices] + constant), ...);
        }

        template <typename T, typename R, size_t... Indices>
        static constexpr inline void _addConstant(__restrict T* data, T constant, std::index_sequence<Indices...>) {
            ((data[Indices] += constant), ...);
        }

        template <typename T, typename R, size_t... Indices>
        static constexpr inline void _sum (const T* __restrict data, T &result, std::index_sequence<Indices...>) {
            ((result += data[Indices]), ...);
        }

        template <typename T, typename R, size_t... Indices>
        static constexpr inline void _dot (const T* a, const T* b, T &result, std::index_sequence<Indices...>) {
            ((result += a[Indices] * b[Indices]), ...);
        }
    };

} // namespace Stalker::Mathematics