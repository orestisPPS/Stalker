#include <iostream>
#include "Tests/STLKR_PerformanceTests/AVX_PerformanceTest.h"
#include "LinearAlgebra/ContiguousMemoryStorage/NumericalVector.h"
#include <chrono>

int main() {
    std::cout << "Hello, World!" << std::endl;

    
    auto exportPath = "../Tests/STLKR_PerformanceTests";
    auto avxTest = STLKR_Tests::RawPointerAVX_PerformanceTest<4000000>(exportPath);
    avxTest.runTest();
    
    

    //STLKR_LinearAlgebra::STLKR_SIMD_Operations<double>::add(data1, data2, result, 4);
    //LinearAlgebra::NumericalVector<double> vector1 = LinearAlgebra::NumericalVector<double>(3);
    return 0;
}
