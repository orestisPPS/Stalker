//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_STLKR_NUMERICALVECTOR_H
#define STALKER_STLKR_NUMERICALVECTOR_H

#include <vector>
using namespace std;

namespace STLKR_LinearAlgebra{
    class STLKR_NumericalVector {
    private:
        vector<double> __atribute__((aligned(32))) _data;
    };
}

#endif //STALKER_STLKR_NUMERICALVECTOR_H
