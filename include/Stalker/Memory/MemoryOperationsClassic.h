#pragma once

#include <cstddef>
#include <cstring>
#include <algorithm>
#include <type_traits>

namespace Stalker::Memory {

    struct MemoryOperationsClassic{
        


        template <typename T, bool UseSTD = true>
        inline static void copy(std::size_t size, T* __restrict destination, const T* __restrict source) noexcept {
            if constexpr (UseSTD) {
                static_assert(std::is_trivially_copyable<T>::value, "memcpy requires trivially copyable T");
                std::memcpy(destination, source, size * sizeof(T));
            } else {
                for (std::size_t i = 0; i < size; ++i)
                    destination[i] = source[i];
            }
        }

        template <typename T, bool UseSTD = true>
        inline static void setValue(size_t size, T* __restrict data, T value) noexcept(std::is_nothrow_copy_assignable<T>::value) {
            if constexpr (UseSTD) {
                std::fill(data, data + size, value);
            } else {
                for (std::size_t i = 0; i < size; ++i)
                    data[i] = value;
            }
        }

        template <typename T, bool UseSTD = true>
        inline static void setZero(std::size_t size, T* __restrict data) noexcept {
            if constexpr (UseSTD) {
                std::memset(data, 0, size * sizeof(T));
            } else {
                for (std::size_t i = 0; i < size; ++i)
                    data[i] = T{};
            }
        }

        template <typename T, bool UseSTD = true>
        inline static bool areEqual(std::size_t size, const T* a, const T* b) noexcept(noexcept(std::declval<const T&>() == std::declval<const T&>())) {
            if constexpr (UseSTD) {
                return std::equal(a, a + size, b);
            } else {
                for (std::size_t i = 0; i < size; ++i)
                    if (!(a[i] == b[i])) return false;
                return true;
            }
        }
    };
};