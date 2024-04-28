#include <iostream>
#include "Tests/AVX_PerformanceTest.h"
#include "LinearAlgebra/ContiguousMemoryStorage/NumericalVector.h"
#include <chrono>

int main() {
    std::cout << "Hello, World!" << std::endl;
    STLKR_Tests::AVX_PerformanceTest::RunPerformanceTestCopy();
    STLKR_Tests::AVX_PerformanceTest::RunPerformanceTestAdd();

    //STLKR_LinearAlgebra::STLKR_SIMD_Operations<double>::add(data1, data2, result, 4);
    //LinearAlgebra::NumericalVector<double> vector1 = LinearAlgebra::NumericalVector<double>(3);
    return 0;
}
