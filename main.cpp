#include <iostream>
#include "Tests/STLKR_PerformanceTests/AVX_PerformanceTest.h"
#include "Tests/STLKR_PerformanceTests/ThreadingPerformanceTest.h"
#include "Tests/StalkerPerformanceVectorTest.h"
#include <chrono>

int main() {
    
    auto exportPath = "../Tests/STLKR_PerformanceTests/logs/Threading";
    //auto threadingTest = STLKR_Tests::ThreadingPerformanceTest<400000, 2>(exportPath);
    auto vectorTest = STLKR_Tests::StalkerPerformanceVectorTest<2>();
    vectorTest.runTest();

 
    //No memory bugs with 40000000 elements
    std::cout << "ΔΩΣΕ ΓΚΑΖΙ ΜΩΡΗ ΑΝΑΠΗΡΗ!!!!!!!!!" << std::endl;
    return 0;
}
