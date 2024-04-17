//
// Created by hal9000 on 3/18/24.
//

#ifndef STALKER_VECTOROPERATIONS_H
#define STALKER_VECTOROPERATIONS_H

#include <cstdio>

namespace LinearAlgebra{
    
    template <typename T>
    class VectorOperations {
    public :
        template<unsigned start, unsigned end>
        constexpr T sumOfElementsUnrolledThreadJob(const T* vector) {
            if constexpr (start < end) { // Ensure recursion stops when start is not less than end
                return vector[start] + sumOfElementsUnrolledThreadJob<start + 1, end>(vector);
            } else {
                return T(0); // Base case: return a zero value of type T
            }
        }


    };
}
#endif //STALKER_VECTOROPERATIONS_H
