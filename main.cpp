#include <iostream>
#include "Tests/StalkerVector_ValidityTests.h"
#include "Tests/STLKR_PerformanceTests/StalkerVector/StalkerVector_PerformanceTests.h"
#include "Tests/STLKR_PerformanceTests/StalkerVector/SingleThreadPerformance/SingleThreadOperationsPerformanceTest.h"
#include "Tests/CPUTopologyTest.h"
#include <chrono>
using namespace STLKR_Tests;
int main() {
    std::cout << "SLET" << std::endl;
    
//    auto readerTest = STLKR_Tests::CPUTopologyTest();
//    readerTest.runTest();
//    
    auto vectorTest = STLKR_Tests::StalkerVector_ValidityTests<2>();
    vectorTest.runTest();
    
auto exportPath = "/home/hal9000/code/stalker/Tests/STLKR_PerformanceTests/StalkerVector/logs";
//auto singleThreadUnrollFactorTest = STLKR_Tests::SingleThreadOperationsPerformanceTest<40000000>(exportPath);
//for (size_t i = 0; i < 20; i++) {
//    if (i % 2 == 0) cout << "Iteration: " << i << endl;
//    singleThreadUnrollFactorTest.runTest();
//}
    std::list<TestConfig_TemplateTypes> typeList {Double, Int};
    std::list<TestConfig_OperationTypes> operationsList{SIMD_Copy, SIMD_Add};
    std::list<unsigned> availableCoresList{1, 2, 4, 8};
    auto vectorPerformanceTest = STLKR_Tests::StalkerVector_PerformanceTests<float, 16>(
            exportPath,
            400000000,
            typeList,
            operationsList,
            availableCoresList,
            EnabledDisabled,
            false,
            Random,
            O3
    );
for (size_t i = 0; i < 1; i++){
    if (i % 1 == 0) cout << "Iteration: " << i << endl;
    vectorPerformanceTest.runTest();
}
    
 
    std::cout << "ΔΩΣΕ ΓΚΑΖΙ ΜΩΡΗ ΑΝΑΠΗΡΗ!!!!!!!!!" << std::endl;
    return 0;
}
