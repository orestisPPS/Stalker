#ifndef LINSPACE_VALIDITYTEST_H
#define LINSPACE_VALIDITYTEST_H

#include "STLKR_TestBase.h"
#include "../Utility/Linspace.h"
#include "TestUtility.h"
#include <array>

namespace STLKR_Tests {

class Linspace_ValidityTests : public STLKR_TestBase {
public:
    explicit Linspace_ValidityTests()
        : STLKR_TestBase("Linspace Validity Tests") {}

    void runTest() override {
        _testRunTime();
        _testCompileTime();
    }

private:
    void _testRunTime() {
        Printers::printSubtitle("Run-time", ColourType::PATSIOURA_RED);
        // 1) Test linspace pointer version
        std::array<double, 5> arrOut{};
        linspace(arrOut.data(), 0.0, 1.0, 5, true);
        constexpr std::array<double, 5> expectedPtr = {0.0, 0.25, 0.5, 0.75, 1.0};
        TestUtility::compareVectors<double>(arrOut.data(), expectedPtr.data(), arrOut.size(),
                                            "Pointer version");

        // 2) Test linspace with std::vector that should resize
        std::vector<double> vecOut;
        linspace(vecOut, 0.0, 2.0, 3, false);
        std::vector<double> expectedVec = {0.0, 2.0/3.0, 4.0/3.0, 2.0};
        TestUtility::compareVectors<double>(vecOut, expectedVec, "Vector version");

        // 3) Single point
        auto singleOut = linspace<>(0.0, 1.0, 1);
        std::vector<double> expectedSingle = {0.0, 1.0};
        TestUtility::compareVectors<double>(singleOut, expectedSingle, "Single point");

        // Negative num
        std::array<double, 1> negOut{};
        try {
            linspace(negOut.data(), 0.0, 1.0, -1, true);
            TestUtility::printFail("Negative num should throw an error");
        }
        catch (const std::invalid_argument&) {
            TestUtility::printPassed("Invalid argument for negative num");
        }

        // Zero num with endpoint=false
        std::vector<double> zeroOut;
        try {
            linspace(zeroOut, 1.0, 1.0, 0, false);
            TestUtility::printFail("Zero num with endpoint=false should throw an error");
        }
        catch (const std::invalid_argument&) {
            TestUtility::printPassed("Invalid argument for zero num with endpoint=false");
        }
    }

    void _testCompileTime() {
        Printers::printSubtitle("Compile-time", ColourType::PATSIOURA_RED);
        // 1) 5 points, endpoint=true
            constexpr auto result1 = linspace<5>(0.0, 1.0, true);
            constexpr std::array<double, 5> expected1 = {0.0, 0.25, 0.5, 0.75, 1.0};
            TestUtility::compareVectors<double>(result1.data(), expected1.data(),result1.size(), 
                                                "5 points (endpoint=true)");
            static_assert(result1 == expected1, "5 points (endpoint=true) Test Failed");

        // 2) 4 points, endpoint=false
            constexpr auto result2 = linspace<4>(0.0, 2.0, false);
            constexpr std::array<double, 4> expected2 = {0.0, 0.5, 1.0, 1.5};
            TestUtility::compareVectors<double>(result2.data(), expected2.data(), result2.size(), 
                                                "4 points (endpoint=false)");
            static_assert(result2 == expected2, "4 points (endpoint=false) Test Failed");


        // 3) Single point, endpoint ignored
            constexpr auto result3 = linspace<1>(5.0, 10.0, false);
            constexpr std::array<double, 1> expected3 = {5.0};
            TestUtility::compareVectors<double>(result3.data(), expected3.data(), result3.size(), "Single point");
            static_assert(result3 == expected3, "Single point Test Failed");
    }
};

} // namespace STLKR_Tests

#endif // LINSPACE_VALIDITYTEST_H