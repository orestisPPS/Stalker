#pragma once

#include <cstddef>
#include <Stalker/Core/Config/LoopUnrolling.h>

namespace Stalker::Mathematics {
    
using namespace Stalker::Core::Config;

    struct MathOperationsMeta {
    public:

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void add(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _add<T, ResultT, false>(a + i, b + i, result + i, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                result[i] = a[i] + b[i];
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void add(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _add<T, ResultT, true>(a + i, b + i, result + i, std::make_index_sequence<Unroll>{}, scalarA, scalarB);
            for (size_t i = limit; i < size; ++i)
                result[i] = (a[i] * scalarA) + (b[i] * scalarB);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void subtract(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _subtract<T, ResultT, false>(a + i, b + i, result + i, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                result[i] = a[i] - b[i];
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void subtract(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _subtract<T, ResultT, true>(a + i, b + i, result + i, std::make_index_sequence<Unroll>{}, scalarA, scalarB);
            for (size_t i = limit; i < size; ++i)
                result[i] = (a[i] * scalarA) - (b[i] * scalarB);
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void multiply(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _multiply<T, ResultT, false>(a + i, b + i, result + i, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                result[i] = a[i] * b[i];
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void multiply(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _multiply<T, ResultT, true>(a + i, b + i, result + i, std::make_index_sequence<Unroll>{}, scalarA, scalarB);
            for (size_t i = limit; i < size; ++i)
                result[i] = (a[i] * scalarA) * (b[i] * scalarB);
        }
        
        // Divide
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void divide(size_t size, const T* a, const T* b, ResultT* result) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _divide<T, ResultT, false>(a + i, b + i, result + i, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                result[i] = a[i] / b[i];
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void divide(size_t size, const T* a, const T* b, ResultT* result, T scalarA, T scalarB) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _divide<T, ResultT, true>(a + i, b + i, result + i, std::make_index_sequence<Unroll>{}, scalarA, scalarB);
            for (size_t i = limit; i < size; ++i)
                result[i] = (a[i] * scalarA) / (b[i] * scalarB);
        }
        // Scale
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void scale(size_t size, const T* __restrict data, ResultT* __restrict result, T scalar) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _scale<T, ResultT>(data + i, result + i, scalar, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                result[i] = data[i] * scalar;
        }

        template <typename T, size_t Unroll = DefaultUnroll()>
        static constexpr inline void scale(size_t size, T* __restrict data, T scalar) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _scale<T, T>(data + i, scalar, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                data[i] *= scalar;
        }

        //needs test
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void addConstant(size_t size, const T* __restrict data, T* __restrict result, T constant) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _addConstant<T, ResultT>(data + i, result + i, constant, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                result[i] = data[i] + constant;
        }
        //needs test
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline void addConstant(size_t size, T* __restrict data, T constant) {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _addConstant<T, ResultT>(data + i, constant, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                data[i] += constant;
        }

        // Sum
        template <typename T, size_t Unroll = DefaultUnroll()>
        static constexpr inline T sum(size_t size, const T* __restrict data) {
            T result = 0;
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _sum<T, T>(data + i, result, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                result += data[i];
            return result;
        }
        // Dot Product
        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline T dot(size_t size, const T* a, const T* b) {
            T result = 0;
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _dot<T, T, false>(a + i, b + i, result, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                result += a[i] * b[i];
            return result;
        }

        template <typename T, size_t Unroll = DefaultUnroll(), typename ResultT = T>
        static constexpr inline T dot(size_t size, const T* a, const T* b, T scalarA, T scalarB) {
            T result = 0;
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _dot<T, ResultT, true>(a + i, b + i, result, std::make_index_sequence<Unroll>{}, scalarA, scalarB);
            for (size_t i = limit; i < size; ++i)
                result += (a[i] * scalarA) * (b[i] * scalarB);
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

        template <typename T, typename R, bool IsScaled, size_t... Indices>
        static constexpr inline void _dot (const T* a, const T* b, T &result, std::index_sequence<Indices...>, T scalarA = 1, T scalarB = 1) {
            if constexpr (IsScaled)
                ((result += (a[Indices] * scalarA) * (b[Indices] * scalarB)), ...);
            else
                ((result += a[Indices] * b[Indices]), ...);
        }
    };

} // namespace Stalker::Mathematics