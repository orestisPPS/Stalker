#include <iostream>
#include "Tests/STLKR_PerformanceTests/AVX_PerformanceTest.h"
#include "Tests/STLKR_PerformanceTests/ThreadingPerformanceTest.h"
#include "Tests/StalkerVector_ValidityTests.h"
#include "Tests/STLKR_PerformanceTests/StalkerVector_PerformanceTests.h"
#include <chrono>

int main() {
    
    auto vectorTest = STLKR_Tests::StalkerVector_ValidityTests<2>();
    vectorTest.runTest();
    
    //auto exportPath = "../Tests/STLKR_PerformanceTests/logs/StalkerVector";
    //auto vectorPerformanceTest = STLKR_Tests::StalkerVector_PerformanceTests<double, 40000000, 16>(exportPath);
    //for (size_t i = 0; i < 20; i++) 
        //vectorPerformanceTest.runTest();
    
 
    std::cout << "ΔΩΣΕ ΓΚΑΖΙ ΜΩΡΗ ΑΝΑΠΗΡΗ!!!!!!!!!" << std::endl;
    return 0;
}
