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
#include <cstring>
#include <algorithm>
#include <type_traits>

namespace Stalker::Memory {

    struct MemoryOperationsClassic{
        


        template <typename T, bool UseSTD = true>
        inline static void copy(size_t size, T* __restrict destination, const T* __restrict source) noexcept {
            if constexpr (UseSTD) {
                static_assert(std::is_trivially_copyable<T>::value, "memcpy requires trivially copyable T");
                std::memcpy(destination, source, size * sizeof(T));
            } else {
                for (size_t i = 0; i < size; ++i)
                    destination[i] = source[i];
            }
        }

        template <typename T, bool UseSTD = true>
        inline static void swap(size_t size, T* __restrict data1, T* __restrict data2) noexcept {
            if constexpr (UseSTD && std::is_trivially_copyable<T>::value) {
                std::swap_ranges(data1, data1 + size, data2);
            } else {
                T temp;
                for (size_t i = 0; i < size; ++i) {
                    temp = data1[i];
                    data1[i] = data2[i];
                    data2[i] = temp;
                }
            }
        }

        template <typename T, bool UseSTD = true>
        inline static void setValue(size_t size, T* __restrict data, T value) noexcept(std::is_nothrow_copy_assignable<T>::value) {
            if constexpr (UseSTD) {
                std::fill(data, data + size, value);
            } else {
                for (size_t i = 0; i < size; ++i)
                    data[i] = value;
            }
        }

        template <typename T, bool UseSTD = true>
        inline static bool areEqual(size_t size, const T* a, const T* b) noexcept(noexcept(std::declval<const T&>() == std::declval<const T&>())) {
            if constexpr (UseSTD) {
                return std::equal(a, a + size, b);
            } else {
                for (size_t i = 0; i < size; ++i)
                    if (!(a[i] == b[i])) return false;
                return true;
            }
        }
    };
};