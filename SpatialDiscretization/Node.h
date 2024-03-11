//
// Created by hal9000 on 3/11/24.
//

#ifndef STALKER_NODE_H

#include<array>
#include <stdexcept>

namespace SpatialDiscretization
{
    template <unsigned dimensions, bool isEngineeringNode = true>
    class Node
    {
    public:
        Node(unsigned id, std::array<double, dimensions> coordinates) : _id(id), _coordinates(coordinates) {
            static_assert(dimensions > 0, "The dimensions of a node must be greater than 0.");
            static_assert(isEngineeringNode && dimensions <= 3, "Engineering nodes must have 3 or fewer dimensions.");
        }
        
        const unsigned& getId() const{ 
            return _id; 
        }
        
        void setCoordinates(std::array<double, dimensions> coordinates) {
            _coordinates = coordinates;
        }
        
        const std::array<double, dimensions>& getCoordinates() const {
            return _coordinates;
        }
        
        std::array<double, dimensions>& getCoordinates3D() {
            switch (dimensions) {
                case 1:
                    return std::array<double, 3>{_coordinates[0], 0.0, 0.0};
                case 2:
                    return std::array<double, 3>{_coordinates[0], _coordinates[1], 0.0};
                case 3:
                    return _coordinates;
                default:
                    throw std::runtime_error("Nodes with more than 3 dimensions can not be returned as 3D coordinates.");
            }
        }
        
    private:
        unsigned _id;
        
        std::array<double, dimensions> _coordinates;
   
    };
}
#define STALKER_NODE_H

#endif //STALKER_NODE_H
