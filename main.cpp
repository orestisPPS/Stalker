#include <iostream>
#include "SpatialDiscretization/Node.h"
#include "LinearAlgebra/ContiguousMemoryStorage/NumericalVector.h"
int main() {
    std::cout << "Hello, World!" << std::endl;
    LinearAlgebra::NumericalVector<double> vector1 = LinearAlgebra::NumericalVector<double>(3);
    return 0;
}
