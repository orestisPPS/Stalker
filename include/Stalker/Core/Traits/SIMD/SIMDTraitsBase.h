#pragma once

#include <immintrin.h>
#include <Stalker/Core/Config/LoopUnrolling.h>
#include <Stalker/Core/Config/Alignment.h>
#include <Stalker/Core/Config/SIMD.h>

namespace Stalker::Core {

using namespace Core::Config;

template<typename T, SIMDType Type, typename Child>
struct SIMDTypeTraitsBase {
    static constexpr unsigned inline RegisterSize() {
        return Child::_RegisterSize;
    }
    static constexpr unsigned inline ElementsPerCacheLine() {
        return STALKER_CACHE_LINE_SIZE / sizeof(typename Child::typeData);
    }

    template<size_t UnrollFactor = DefaultUnrollFactor()>
    static constexpr unsigned inline CacheLinesProcessed() { 
        return (UnrollFactor * Child::_RegisterSize) / ElementsPerCacheLine(); 
    }

    template<size_t UnrollFactor = DefaultUnrollFactor()>
    static constexpr unsigned inline BlockSize() {
        return Child::_RegisterSize * UnrollFactor;
    }
};

    template<typename T, SIMDType simdType> struct SIMDTypeTraits;

} // namespace Stalker::Core
