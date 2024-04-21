//
// Created by hal9000 on 4/21/24.
//

#ifndef STALKER_STLKR_ARITHMETICTYPES_H
#define STALKER_STLKR_ARITHMETICTYPES_H
#include <map>
namespace STLKR_LinearAlgebra{
    enum STLK_NumericDataType {
        STLKR_FLOAT,
        STLKR_DOUBLE,
        STLKR_LONG_DOUBLE,
        STLKR_INT,
        STLKR_LONG,
        STLKR_UNSIGNED,
    };
    
    const static std::map<STLK_NumericDataType, unsigned> STLKR_NumericDataTypeSize = {
        {STLKR_FLOAT, sizeof(float)},
        {STLKR_DOUBLE, sizeof(double)},
        {STLKR_LONG_DOUBLE, sizeof(long double)},
        {STLKR_INT, sizeof(int)},
        {STLKR_LONG, sizeof(long)},
        {STLKR_UNSIGNED, sizeof(unsigned)}
    };
}
#endif //STALKER_STLKR_ARITHMETICTYPES_H
