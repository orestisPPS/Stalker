#ifndef STALKER_TESTS_STALKER_TEST_RUNNER_H
#define STALKER_TESTS_STALKER_TEST_RUNNER_H

// #include "StalkerMatrix/IteratorTests.h"
// #include "StalkerMatrix/FixedStridePtrBufferTests.h"
// #include "StalkerMatrix/DataAccessTests.h"
#include "Mathematics/LinspaceTests.h"
#include "Mathematics/MetaMathTests.h"
#include "Mathematics/IntegrationTests.h"
#include "Mathematics/DifferentiationTests.h"
#include "Memory/AllocatorsTests.h"
#include "Memory/MemoryOperationsTests.h"
#include "Mathematics/MathOperationsTests.h"
#include "Mathematics/MatrixMathTests.h"

namespace STLKR_Tests {
    class StalkerTestRunner {
    public:
        StalkerTestRunner() = default;
        ~StalkerTestRunner() = default;

        void runTests() {
            std::cout << "SLET" << std::endl;

            LinspaceTests().runTest();
            MetaMathTests<double>().runTest();
            IntegrationTests().runTest();
            DifferentiationTests().runTest();
            TypeTraitsSIMDTests().runTest();
            AllocatorsTests().runTest();
            MemoryOperationsTests(2'000).runTest();
            MathOperationsTests(2'000).runTest();
            MatrixMathTests(333003).runTest();

            // IteratorTests().runTest();
            // FixedStridePtrBufferTests().runTest(); //NOT HOMOGENEOUS
            // DataAccessTests<double>().runTest();
            std::cout << "SLET" << std::endl;
        }
    };
}

#endif // STALKER_TESTS_STALKER_TEST_RUNNER_H