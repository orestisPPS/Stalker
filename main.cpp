#include <iostream>
#include "SpatialDiscretization/Node.h"
#include "LinearAlgebra/ContiguousMemoryStorage/STLKR_SIMD_Operations.h"
#include "LinearAlgebra/ContiguousMemoryStorage/NumericalVector.h"
int main() {
    std::cout << "Hello, World!" << std::endl;
    auto data1 = new double[4]{1.0, 2.0, 3.0, 4.0};
    auto data2 = new double[4]{1.0, 2.0, 3.0, 4.0};
    auto result = new double[4];
    STLKR_LinearAlgebra::STLKR_SIMD_Operations<double>::add(data1, data2, result, 4);
    //LinearAlgebra::NumericalVector<double> vector1 = LinearAlgebra::NumericalVector<double>(3);
    return 0;
}
