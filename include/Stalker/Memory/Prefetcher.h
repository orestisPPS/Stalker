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
#include <Stalker/Core/Config/LoopUnrolling.h>
#include <Stalker/Core/Config/SIMD.h>
#include <utility>

namespace Stalker::Memory {
    

    struct Prefetcher{

        template<typename T, T_PrefetchHints Hint, size_t Unroll>
        inline static void prefetch(const T* ptr) {
            if constexpr (Hint == T_PrefetchHints::HintT0) {
                _prefetchT0(ptr, UnrollIndexSequence<Unroll>{});
            }
            else if constexpr (Hint == T_PrefetchHints::HintT1) {
                _prefetchT1(ptr, UnrollIndexSequence<Unroll>{});
            }
            else if constexpr (Hint == T_PrefetchHints::HintT2) {
                _prefetchT2(ptr,  UnrollIndexSequence<Unroll>{});
            }
            else if constexpr (Hint == T_PrefetchHints::HintNTA) {
                _prefetchNTA(ptr,  UnrollIndexSequence<Unroll>{});
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