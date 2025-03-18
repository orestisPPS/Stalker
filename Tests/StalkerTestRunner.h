#ifndef STALKER_TESTS_STALKER_TEST_RUNNER_H
#define STALKER_TESTS_STALKER_TEST_RUNNER_H

#include <iostream>
// #include "StalkerVector_ValidityTests.h"
// #include "STLKR_PerformanceTests/StalkerVector/StalkerVector_PerformanceTests.h"
// #include "STLKR_PerformanceTests/StalkerVector/SingleThreadPerformance/SingleThreadOperationsPerformanceTest.h"
#include "StalkerMatrix/Iterator_ValidityTests.h"
#include "StalkerMatrix/FixedStridePtrBuffer_ValidityTest.h"
#include "../DataStructures/Concepts/NumericTypeConstraints.h"
#include "StalkerMatrix/DataAccess_ValidityTests.h"
#include "StalkerMathematics/MetaMath_ValidityTests.h"
#include "StalkerMathematics/Differentiation_ValidityTests.h"
#include "StalkerMathematics/Integration_ValidityTests.h"
#include "Linspace_ValidityTest.h"
#include "Exporter_ValidityTest.h"
#include "CPUTopologyTest.h"
#include "../Utility/Checkers.h"
#include <chrono>
#include <ostream>


namespace STLKR_Tests {
    class StalkerTestRunner {
    public:
        StalkerTestRunner() = default;
        ~StalkerTestRunner() = default;

        void runTests() {
                std::cout << "SLET" << std::endl;
        
        //    auto readerTest = STLKR_Tests::CPUTopologyTest();
        //    readerTest.runTest();
        //    
            //STLKR_Tests::StalkerVector_ValidityTests<2>().runTest();

        Linspace_ValidityTests().runTest();
        MatrixBlockIteratorTest().runTest();
        MatrixBuffers_ValidityTests().runTest(); //NOT HOMOGENEOUS
        DataAccess_ValidityTests<double>().runTest();
        MetaprogrammingMath_ValidityTests<double>().runTest();
        Integration_ValidityTests().runTest();
        Differentiation_ValidityTests().runTest();
        STLKR_Tests::Exporter_ValidityTests().runTest();

        
        auto exportPath = "/home/hal9000/code/stalker/Tests/STLKR_PerformanceTests/StalkerVector/logs";
        //auto singleThreadUnrollFactorTest = STLKR_Tests::SingleThreadOperationsPerformanceTest<40000000>(exportPath);
        //for (size_t i = 0; i < 20; i++) {
        //    if (i % 2 == 0) cout << "Iteration: " << i << endl;
        //    singleThreadUnrollFactorTest.runTest();
        //}
        // std::list<TestConfig_OperationTypes> operationsList{SIMD_Copy};
        // std::list<unsigned> availableCoresList{1, 2, 4};
        // std::list<CoreAffinityConfig> affinities{HT_CoreSet, HT_PoolSet};

        // STLKR_Tests::StalkerVector_PerformanceTests<float, 16>(
        //         exportPath,                // std::string &path
        //         5,
        //         800000000,                 // unsigned size
        //         operationsList,            // std::list<TestConfig_OperationTypes> &operations
        //         availableCoresList,        // std::list<unsigned> &availableCores
        //         affinities,                // std::list<CoreAffinityConfig> &threadAffinities
        //         false,                     // bool prefetching
        //         Index,                    // TestConfig_FillType fillType
        //         O0                         // TestConfig_CompilerFlag flag
        // ).runTest();

        }
    };
}

#endif // STALKER_TESTS_STALKER_TEST_RUNNER_H