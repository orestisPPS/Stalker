#include <iostream>
#include "SpatialDiscretization/Node.h"
int main() {
    std::cout << "Hello, World!" << std::endl;
    
    auto coords = std::array<double, 3>{1.0, 2.0, 3.0};
    SpatialDiscretization::Node<3> node(1, coords);
    return 0;
}
