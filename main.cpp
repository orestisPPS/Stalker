#include <iostream>
#include "SpatialDiscretization/Node.h"
#include "LinearAlgebra/ContiguousMemoryStorage/STLKR_EnumerableMathematicalOperations/STLKR_SIMD_Operations.h"
#include "LinearAlgebra/ContiguousMemoryStorage/STLKR_Enumerable/STLKR_SIMD/STLKR_SIMD_DataOperations.h"
#include "LinearAlgebra/ContiguousMemoryStorage/NumericalVector.h"
using namespace STLKR_LinearAlgebra;

int main() {
    std::cout << "Hello, World!" << std::endl;
    auto data1 = new double[4]{1.0, 2.0, 3.0, 4.0};
    auto data2 = new double[4]{6.0, 6.0, 3.0, 1.0};
    auto result = new double[4];
    
    auto simdPolicy = STLKR_SIMD(STLKR_LinearAlgebra::SIMD_AVXType::AVX_2, SIMD_AlignmentSize::AlignmentSize_256);
    STLKR_SIMD_DataOperations<double, 4>::deepcopy(data1, data2, simdPolicy);
    for (int i = 0; i < 4; i++) {
        std::cout << data1[i] << " ";
    }
    //STLKR_LinearAlgebra::STLKR_SIMD_Operations<double>::add(data1, data2, result, 4);
    //LinearAlgebra::NumericalVector<double> vector1 = LinearAlgebra::NumericalVector<double>(3);
    return 0;
}
