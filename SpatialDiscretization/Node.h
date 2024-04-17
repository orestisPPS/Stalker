// Node.h
#ifndef STALKER_NODE_H
#define STALKER_NODE_H

#include <array>
#include <stdexcept>

namespace SpatialDiscretization {
    template <unsigned dimensions, bool isEngineeringNode = true>
    class Node {
    public:

        explicit Node(unsigned id) : _id(id), _coordinates() {}

        // Move constructor
        Node(Node&& other) noexcept : _id(std::exchange(other._id, 0)), _coordinates(std::move(other._coordinates)) {}

        // Copy constructor
        Node(const Node& other) : _id(other._id), _coordinates(other._coordinates) {}

        // Copy assignment operator with self-assignment check
        Node& operator=(const Node& other) {
            if (this != &other) {
                _id = other._id;
                _coordinates = other._coordinates;
            }
            return *this;
        }

        // Move assignment operator
        Node& operator=(Node&& other) noexcept {
            if (this != &other) {
                _id = std::exchange(other._id, 0);
                _coordinates = std::move(other._coordinates);
            }
            return *this;
        }

        // Operators
        bool operator==(const Node& other) const {
            return (_id == other._id && _coordinates == other._coordinates);
        }

        bool operator!=(const Node& other) const {
            return *this != other;
        }

        [[nodiscard]] constexpr const unsigned& getId() const {
            return _id;
        }

        void setCoordinates(std::array<double, dimensions> coordinates) {
            _coordinates = coordinates;
        }

        constexpr const std::array<double, dimensions>& getCoordinates() const {
            return _coordinates;
        }

        [[nodiscard]] constexpr std::array<double, 3> getCoordinates3D() const {
            std::array<double, 3> coordinates3D{0.0, 0.0, 0.0};
            if constexpr (dimensions == 1) coordinates3D[0] = _coordinates[0];
            if constexpr (dimensions == 2) coordinates3D[1] = _coordinates[1];
            if constexpr (dimensions == 3) coordinates3D[2] = _coordinates[2];
            if constexpr (dimensions > 3) throw std::out_of_range("The number of dimensions is greater than 3.");
            return coordinates3D;
        }

    private:
        unsigned _id;
        std::array<double, dimensions> _coordinates;
        static_assert(dimensions > 0, "The dimensions of a node must be greater than 0.");
        static_assert(isEngineeringNode && dimensions <= 3, "Engineering nodes must have 3 or fewer dimensions.");
    };
}

#endif //STALKER_NODE_H
