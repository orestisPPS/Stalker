//
// Created by hal9000 on 3/11/24.
//

#ifndef STALKER_MESHFACTORY_H
#define STALKER_MESHFACTORY_H

#include "MeshSpecs.h"
namespace SpatialDiscretization {
    
    template <unsigned dimensions, unsigned numberOfNodes>
    class MeshFactory {
        public:
        MeshFactory(MeshSpecs<dimensions, numberOfNodes> meshSpecs) {
           
        }
        private:
            std::array< double, dimensions * numberOfNodes > _naturalCoordinates;
            std::array< double, dimensions * numberOfNodes > _parametricCoordinates;
            std::array< double, dimensions * numberOfNodes > _templateCoordinates;
            MeshSpecs<dimensions, numberOfNodes> _meshSpecs;
            
            void _calculateParametricAndTemplateCoordinates() {

                // Calculate parametric coordinates
                for (unsigned i = 0; i < numberOfNodes; i++) {
                    for (unsigned j = 0; j < dimensions; j++) {
                        _parametricCoordinates[i * dimensions + j] = j;

                    }
                }
            }
    };

} // SpatialDiscretization

#endif //STALKER_MESHFACTORY_H
