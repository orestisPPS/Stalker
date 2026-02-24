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
#include <Stalker/Core/Config/Platform.h>
#include <Stalker/Core/Config/SIMD.h>
#include <Stalker/Core/Config/Compiler.h>

namespace Stalker::Core {

using namespace Core::Config;

template<typename T, T_SIMD Type, typename Child>
struct TypeTraitsSIMDBase {
    static constexpr unsigned inline RegisterSize() {
        return Child::_RegisterSize;
    }
    static constexpr unsigned inline ElementsPerCacheLine() {
        return CacheLineSize() / sizeof(typename Child::typeData);
    }

    template<size_t Unroll = DefaultUnroll()>
    static constexpr unsigned inline CacheLinesPerBlock() { 
        return (Unroll * Child::_RegisterSize + ElementsPerCacheLine() - 1) / ElementsPerCacheLine(); 
    }

    template<size_t Unroll = DefaultUnroll()>
    static constexpr unsigned inline BlockSize() {
        return Child::_RegisterSize * Unroll;
    }
};

template<typename T, T_SIMD simdType> struct TypeTraitsSIMD;

} // namespace Stalker::Core
