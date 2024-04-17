//
// Created by hal9000 on 3/11/24.
//

#ifndef STALKER_CURVILINEARMESH_H
#define STALKER_CURVILINEARMESH_H
#include "Node.h"
namespace SpatialDiscretization {
    template <unsigned dimensions, unsigned numberOfNodes>
    class CurvilinearMesh {
    public:
        CurvilinearMesh() {
            blockOfNodes = new char[sizeof(Node<dimensions>) * numberOfNodes];
            for (unsigned i = 0; i < numberOfNodes; i++) {
                _nodes[i] = Node<dimensions>(i, std::array<double, dimensions>{0.0});
            }
        }
    private:
        std::array<Node<dimensions>, numberOfNodes> _nodes;
        char *blockOfNodes;
    };

} // SpatialDiscretization

#endif //STALKER_CURVILINEARMESH_H
