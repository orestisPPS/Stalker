#pragma once
#include <Stalker/Core/Config/LoopUnrolling.h>
#include <Stalker/Core/Config/SIMD.h>
#include <utility>

namespace Stalker::Memory {
    

    struct Prefetcher{

        template<typename T, T_PrefetchHints Hint, size_t Unroll>
        inline static void prefetch(const T* ptr) {
            if constexpr (Hint == T_PrefetchHints::HintT0) {
                _prefetchT0(ptr, std::make_index_sequence<Unroll>{});
            }
            else if constexpr (Hint == T_PrefetchHints::HintT1) {
                _prefetchT1(ptr, std::make_index_sequence<Unroll>{});
            }
            else if constexpr (Hint == T_PrefetchHints::HintT2) {
                _prefetchT2(ptr,  std::make_index_sequence<Unroll>{});
            }
            else if constexpr (Hint == T_PrefetchHints::HintNTA) {
                _prefetchNTA(ptr,  std::make_index_sequence<Unroll>{});
            }
        }
    
    private:
        template<typename T, size_t... Is>
        inline static void _prefetchT0(const T* ptr, std::index_sequence<Is...>) {
            ( _mm_prefetch(ptr + Is, _MM_HINT_T0), ... );
        }
        template<typename T, size_t... Is>
        inline static void _prefetchT1(const T* ptr, std::index_sequence<Is...>) {
            ( _mm_prefetch(ptr + Is, _MM_HINT_T1), ... );
        }
        template<typename T, size_t... Is>
        inline static void _prefetchT2(const T* ptr, std::index_sequence<Is...>) {
            ( _mm_prefetch(ptr + Is, _MM_HINT_T2), ... );
        }
        template<typename T, size_t... Is>
        inline static void _prefetchNTA(const T* ptr, std::index_sequence<Is...>) {
            ( _mm_prefetch(ptr + Is, _MM_HINT_NTA), ... );
        }  
    };

}