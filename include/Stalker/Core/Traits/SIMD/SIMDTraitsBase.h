#pragma once

#include <immintrin.h>
#include <Stalker/Core/Config/LoopUnrolling.h>
#include <Stalker/Core/Config/Alignment.h>

namespace Stalker::Core {

enum class SIMDType {
    SSE,
    AVX,
    AVX2,
    AVX512
};

template<typename T, SIMDType Type, typename Child>
struct SIMDTypeTraitsBase {
    static constexpr unsigned RegisterSize() {
        return Child::_RegisterSize;
    }
    static constexpr unsigned ElementsPerCacheLine() {
        return STALKER_CACHE_LINE_SIZE / sizeof(typename Child::typeData);
    }

    template<size_t UnrollFactor = STALKER_UNROLL_FACTOR>
    static constexpr unsigned CacheLinesProcessed() { 
        return (UnrollFactor * Child::_RegisterSize) / ElementsPerCacheLine(); 
    }

    template<size_t UnrollFactor = STALKER_UNROLL_FACTOR>
    static constexpr unsigned BlockSize() {
        return Child::_RegisterSize * UnrollFactor;
    }
};

    template<typename T, SIMDType simdType> struct SIMDTypeTraits;

} // namespace Stalker::Core
