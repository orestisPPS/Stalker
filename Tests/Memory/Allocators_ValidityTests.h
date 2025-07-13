#pragma once

#include "STLKR_TestBase.h"
#include <Stalker/Memory/Allocators.h>
#include "TestUtility.h"
#include <vector>
#include <memory>
#include <limits>
#include <cstdint>
#include <cstring>          // std::memset
#include <Stalker/Memory/Allocators.h>

namespace STLKR_Tests {

using namespace Stalker::Memory;
using TU = TestUtility;     // brevity

class Allocators_ValidityTests : public STLKR_TestBase {
public:
    Allocators_ValidityTests() : STLKR_TestBase("Memory Allocators Validity Tests") {}

    /*--------------------------------------------------------------
     *  1.  Helpers
     *--------------------------------------------------------------*/
    template<typename T> static bool isAligned(const T* p, std::size_t A)
    { return (reinterpret_cast<std::uintptr_t>(p) & (A - 1)) == 0; }

    template<typename T, std::size_t A>
    void roundTripRaw(std::size_t n)
    {
        auto* p = AlignedAllocator<T, A>::allocate(n);
        bool ok1 = TU::compareValues(isAligned(p, A), true, "alignment");
        bool ok2 = TU::compareValues(p != nullptr,      true, "non-null");

        /* write / read smoke test */
        for (std::size_t i = 0; i < n; ++i) p[i] = static_cast<T>(i);
        bool ok3 = TU::compareValues(p[n-1], static_cast<T>(n-1), "R/W check", true);

        AlignedAllocator<T, A>::deallocate(p);
        (void)(ok1 && ok2 && ok3);
    }

    /*--------------------------------------------------------------
     *  2.  Main test body
     *--------------------------------------------------------------*/
    void runTest() override
    {
        printSubtitle("AlignedAllocator int 16",    ColourType::BARBIE_PINK);
        roundTripRaw<int,    16>(128);
        printSubtitle("AlignedAllocator float 32",  ColourType::BARBIE_PINK);
        roundTripRaw<float,  32>(256);
        printSubtitle("AlignedAllocator double 64", ColourType::BARBIE_PINK);
        roundTripRaw<double, 64>(512);
        printSubtitle("AlignedAllocator char 128",  ColourType::BARBIE_PINK);
        roundTripRaw<char,  128>(1024); 
        
        printSubtitle("Edge Cases",  ColourType::BARBIE_PINK);
        /* zero-size allocation */
        {
            int* p = AlignedAllocator<int, 16>::allocate(0);
            TU::compareValues(p == nullptr, true, "allocate(0) returns nullptr");
            AlignedAllocator<int, 16>::deallocate(p);
        }
        
        /* expected std::bad_alloc */
        {
            try {
                auto* p = AlignedAllocator<char, 16>::allocate(std::numeric_limits<std::size_t>::max()/8);
                (void)p;                               // should never hit
                TU::printFail("huge allocation did NOT throw");
            } catch (const std::bad_alloc&) {
                TU::printPassed("huge allocation throws std::bad_alloc");
            }
        }
        
        printSubtitle("Allocation Helpers", ColourType::BARBIE_PINK);
        
        {
            std::vector<float, AlignedAllocator<float, 32>> v(1000, 1.0f);
            v.push_back(2.0f);
            bool ok = TU::compareValues(v.back(), 2.0f, "Aligned std::vector with custom allocator");
            (void)ok;
        }


        /* raw */
        {
            auto* p = createAlignedRaw<double, 32>(12);
            bool ok = TU::compareValues(isAligned(p, 32), true, "Raw ptr");
            AlignedAllocator<double, 32>::deallocate(p);
            (void)ok;
        }

        /* unique_ptr */
        {
            auto up = createAlignedUnique<int, 64>(33);
            bool ok = TU::compareValues(isAligned(up.get(), 64), true, "Unique ptr");
            up.reset();          // implicit dealloc
            (void)ok;
        }

        /* shared_ptr */
        {
            auto sp = createAlignedShared<float, 32>(25);
            bool ok = TU::compareValues(isAligned(sp.get(), 32), true, "Shared ptr");
            sp.reset();
            (void)ok;
        }

        /* C-style array under unique_ptr<T[]> */
        {
            auto arr = createAlignedArray<char, 16>(256);
            bool ok1 = TU::compareValues(isAligned(arr.get(), 16), true, "Array");
            std::memset(arr.get(), 0xAB, 256);
            bool ok2 = TU::compareValues(arr[0], static_cast<char>(0xAB), "array R/W smoke", true);
            arr.reset();
            (void)(ok1 && ok2);
        }
    }
};

} // namespace STLKR_Tests
