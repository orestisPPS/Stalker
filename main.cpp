#include <iostream>
#include "Tests/STLKR_PerformanceTests/AVX_PerformanceTest.h"
#include "LinearAlgebra/ContiguousMemoryStorage/NumericalVector.h"
#include <chrono>

int main() {
    std::cout << "Hello, World!" << std::endl;

    
    auto exportPath = "../Tests/STLKR_PerformanceTests/logs";
    //No memory bugs with 40000000 elements
    auto avxTest = STLKR_Tests::RawPointerAVX_PerformanceTest<4000000>(exportPath);
    for (int i = 0; i < 150; i++) {
        avxTest.runTest();
        //cout<<"He not like us, he not like us, he not like us" << endl;
        //cout<<"He not like us, he not like us, he not like us" << endl;
    }
    
    

    //STLKR_LinearAlgebra::STLKR_SIMD_Operations<double>::add(data1, data2, result, 4);
    //LinearAlgebra::NumericalVector<double> vector1 = LinearAlgebra::NumericalVector<double>(3);
    return 0;
}
