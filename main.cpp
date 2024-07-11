#include <iostream>
#include "Tests/STLKR_PerformanceTests/AVX_PerformanceTest.h"
#include "Tests/STLKR_PerformanceTests/ThreadingPerformanceTest.h"
#include "Tests/StalkerVector_ValidityTests.h"
#include "Tests/STLKR_PerformanceTests/StalkerVector_PerformanceTests.h"
#include <chrono>

int main() {
    
    //auto threadingTest = STLKR_Tests::ThreadingPerformanceTest<400000, 2>(exportPath);
    auto vectorTest = STLKR_Tests::StalkerVector_ValidityTests<2>();
    vectorTest.runTest();
    
    auto exportPath = "../Tests/STLKR_PerformanceTests/logs/StalkerVector";
    auto vectorPerformanceTest = STLKR_Tests::StalkerVector_PerformanceTests<float, 40000000, 2>(exportPath);
    for (size_t i = 0; i < 1; i++) {
        vectorPerformanceTest.runTest();
    }
 
    //No memory bugs with 40000000 elements
    std::cout << "ΔΩΣΕ ΓΚΑΖΙ ΜΩΡΗ ΑΝΑΠΗΡΗ!!!!!!!!!" << std::endl;
    return 0;
}
