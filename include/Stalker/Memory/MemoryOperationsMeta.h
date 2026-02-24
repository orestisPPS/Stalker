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

#include <utility>
#include <Stalker/Core/Config/LoopUnrolling.h>

namespace Stalker::Memory {
    
using namespace Stalker::Core::Config;

    struct MemoryOperationsMeta {
    public:

        template <typename T, size_t Unroll = DefaultUnroll()>
        STALKER_FORCE_INLINE constexpr static void copy(size_t size, T* STALKER_RESTRICT destination, const T* STALKER_RESTRICT source )  {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _copy<T>(destination + i, source + i, UnrollIndexSequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                destination[i] = source[i];
        }
        
        template <typename T, size_t Unroll = DefaultUnroll()>
        STALKER_FORCE_INLINE constexpr static void swap(size_t size, T* STALKER_RESTRICT data1, T* STALKER_RESTRICT data2)  {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _swap<T>(data1 + i, data2 + i, UnrollIndexSequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                std::swap(data1[i], data2[i]);
        }

        template <typename T, size_t Unroll = DefaultUnroll()>
        STALKER_FORCE_INLINE constexpr static void setValue(size_t size, T* STALKER_RESTRICT data, T value)  {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _setValue<T>(data  + i, value, UnrollIndexSequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                data[i] = value;
        }
       
    private:
        template <typename T, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _copy(T* STALKER_RESTRICT destination, const T* STALKER_RESTRICT source, std::index_sequence<Indices...>) {
            ((destination[Indices] = source[Indices]), ...);

        }

        template <typename T, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _swap(T* STALKER_RESTRICT data1, T* STALKER_RESTRICT data2, std::index_sequence<Indices...>) {
            T temp[sizeof...(Indices)];
            ((temp[Indices] = data1[Indices]), ...);
            ((data1[Indices] = data2[Indices]), ...);
            ((data2[Indices] = temp[Indices]), ...);
        }

        template <typename T, size_t... Indices>
        STALKER_FORCE_INLINE constexpr static void _setValue(T* STALKER_RESTRICT data, T value, std::index_sequence<Indices...>) {
            ((data[Indices] = value), ...);
        }
    };

} // namespace Stalker::Mathematics