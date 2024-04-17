//
// Created by hal9000 on 3/11/24.
//

#ifndef STALKER_MESHSPECS_H
#define STALKER_MESHSPECS_H
#include <array>


template <unsigned dimensions, unsigned numberOfNodes>
class MeshSpecs {
    public:
    MeshSpecs(std::array<unsigned, dimensions> nodesPerDirection,
              std::array<double, dimensions> lengthPerDirection,
              std::array<double, dimensions> rotationAnglePerDirection,
              std::array<double, dimensions> shearAnglePerDirection) :
              _nodesPerDirection(nodesPerDirection),
              _lengthPerDirection(lengthPerDirection),
              _rotationAnglePerDirection(rotationAnglePerDirection),
              _shearAnglePerDirection(shearAnglePerDirection) {
        
    }
    
    private:
    std::array<unsigned, dimensions> _nodesPerDirection;
    std::array<double, dimensions> _lengthPerDirection;
    std::array<double, dimensions> _rotationAnglePerDirection;
    std::array<double, dimensions> _shearAnglePerDirection;
};


#endif //STALKER_MESHSPECS_H
