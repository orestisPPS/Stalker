#pragma once

#include <Stalker/Core/Config/LoopUnrolling.h>

namespace Stalker::Memory {
    
using namespace Stalker::Core::Config;

    struct MemoryOperationsMeta {
    public:

        template <typename T, size_t Unroll = DefaultUnroll()>
        static constexpr inline void copy(size_t size, T* __restrict destination, const T* __restrict source )  {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _copy<T>(destination + i, source + i, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                destination[i] = source[i];
        }

        template <typename T, size_t Unroll = DefaultUnroll()>
        static constexpr inline void setValue(size_t size, T* __restrict data, T value)  {
            auto limit = size - (size % Unroll);
            for (size_t i = 0; i < limit; i += Unroll)
                _setValue<T>(data  + i, value, std::make_index_sequence<Unroll>{});
            for (size_t i = limit; i < size; ++i)
                data[i] = value;
        }
        
        template <typename T, size_t Unroll = DefaultUnroll()>
        static constexpr inline void setZero(size_t size, T* __restrict data)  {
            setValue<T, Unroll>(size, data, static_cast<T>(0));
        }
       
    private:
        template <typename T, size_t... Indices>
        static constexpr inline void _copy(T* __restrict destination, const T* __restrict source, std::index_sequence<Indices...>) {
            ((destination[Indices] = source[Indices]), ...);

        }

        template <typename T, size_t... Indices>
        static constexpr inline void _setValue(T* __restrict data, T value, std::index_sequence<Indices...>) {
            ((data[Indices] = value), ...);
        }
    };

} // namespace Stalker::Mathematics