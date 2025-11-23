#ifndef STALKER_TESTS_STALKER_TEST_RUNNER_H
#define STALKER_TESTS_STALKER_TEST_RUNNER_H

#include <iostream>
// #include "StalkerMatrix/Iterator_ValidityTests.h"
// #include "StalkerMatrix/FixedStridePtrBuffer_ValidityTest.h"
// #include "StalkerMatrix/DataAccess_ValidityTests.h"
#include "Mathematics/Linspace_ValidityTest.h"
#include "Mathematics/MetaMath_ValidityTests.h"
#include "Mathematics/Integration_ValidityTests.h"
#include "Mathematics/Differentiation_ValidityTests.h"
#include "Core/TraitsSIMD_ValidityTests.h"
#include "Memory/Allocators_ValidityTests.h"
#include "Memory/MemoryOperations_ValidityTests.h"
#include "Mathematics/MathOperations_ValidityTests.h"

#include <ostream>


namespace STLKR_Tests {
    class StalkerTestRunner {
    public:
        StalkerTestRunner() = default;
        ~StalkerTestRunner() = default;

        void runTests() {
            std::cout << "SLET" << std::endl;
            
            Linspace_ValidityTests().runTest();
            MetaprogrammingMath_ValidityTests<double>().runTest();
            Integration_ValidityTests().runTest();
            Differentiation_ValidityTests().runTest();
            TraitsSIMD_ValidityTests().runTest();
            Allocators_ValidityTests().runTest();
            MemoryOperations_ValidityTests(2'000).runTest();
            MathOperations_ValidityTests(2'000).runTest();
            
            // MatrixBlockIteratorTest().runTest();
            // MatrixBuffers_ValidityTests().runTest(); //NOT HOMOGENEOUS
            // DataAccess_ValidityTests<double>().runTest();
            std::cout << "SLET" << std::endl;
        }
    };
}

#endif // STALKER_TESTS_STALKER_TEST_RUNNER_H