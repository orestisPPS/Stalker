//
// Created by hal9000 on 3/11/24.
//

#ifndef STALKER_NODALCOORDINATES_H
#define STALKER_NODALCOORDINATES_H


namespace SpatialDiscretization{

    /**
    * \enum CoordinateType
    * \brief Enumeration for defining the type of nodal coordinates. Can be natural, parametric, or template.
    */
    enum CoordinateType {
        Natural,      ///< Natural coordinates.
        Parametric,   ///< Parametric coordinates.
        Template      ///< Template coordinates.
    };
    
    template <unsigned dimensions>
    class NodalCoordinates
    {
    public:
        NodalCoordinates();
        
        void setCoordinates(array<double, dimensions> coordinates);
        
        const array<double, dimensions>& getCoordinates() const;
        
        array<double, dimensions>& getCoordinates3D();
        
        
    private:
        array<double, dimensions> _coordinates;
    };
}
#endif //STALKER_NODALCOORDINATES_H
