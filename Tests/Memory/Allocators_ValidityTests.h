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
        printSubtitle("AlignedAllocator int 16",    T_Color::BARBIE_PINK);
        roundTripRaw<int,    16>(128);
        printSubtitle("AlignedAllocator float 32",  T_Color::BARBIE_PINK);
        roundTripRaw<float,  32>(256);
        printSubtitle("AlignedAllocator double 64", T_Color::BARBIE_PINK);
        roundTripRaw<double, 64>(512);
        printSubtitle("AlignedAllocator char 128",  T_Color::BARBIE_PINK);
        roundTripRaw<char,  128>(1024); 
        
        printSubtitle("Edge Cases",  T_Color::BARBIE_PINK);
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
        
        printSubtitle("Allocation Helpers", T_Color::BARBIE_PINK);
        
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

        /* std::vector with AlignedAllocator */
        {
            auto vec = createAlignedVector<double, 64>(128);
            bool ok1 = TU::compareValues(isAligned(vec.data(), 64), true, "std::vector data alignment");
            vec[127] = 3.14;
            bool ok2 = TU::compareValues(vec[127], 3.14, "std::vector R/W smoke", true);
            (void)(ok1 && ok2);
        }
        /* std::vector with AlignedAllocator, with reallocations */
        {
            std::vector<int, AlignedAllocator<int, 32>> vec;
            vec.reserve(10);
            bool ok1 = TU::compareValues(isAligned(vec.data(), 32), true, "std::vector alignment after reserve");

            for (int i = 0; i < 100; ++i)
            {
            vec.push_back(i);
            }
            bool ok2 = TU::compareValues(isAligned(vec.data(), 32), true, "std::vector alignment after push_back reallocs");
            bool ok3 = TU::compareValues(vec.size(), (size_t)100, "std::vector size check");
            bool ok4 = TU::compareValues(vec[99], 99, "std::vector R/W check after reallocs", true);

            vec.resize(200);
            bool ok5 = TU::compareValues(isAligned(vec.data(), 32), true, "std::vector alignment after resize");
            bool ok6 = TU::compareValues(vec.size(), (size_t)200, "std::vector size check after resize");
            (void)(ok1 && ok2 && ok3 && ok4 && ok5 && ok6);
        }
        
    }
};

} // namespace STLKR_Tests
