#pragma once

#include <Stalker/Core/Config/LoopUnrolling.h>
#include <Stalker/Core/Config/Alignment.h>
#include <Stalker/Core/Config/Platform.h>
#include <Stalker/Core/Config/SIMD.h>

namespace Stalker::Core {

using namespace Core::Config;

template<typename T, SIMDType Type, typename Child>
struct TypeTraitsSIMDBase {
    static constexpr unsigned inline RegisterSize() {
        return Child::_RegisterSize;
    }
    static constexpr unsigned inline ElementsPerCacheLine() {
        return CacheLineSize() / sizeof(typename Child::typeData);
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

    template<typename T, SIMDType simdType> struct TypeTraitsSIMD;

} // namespace Stalker::Core
