#ifndef FULLMATRIXDATA_H
#define FULLMATRIXDATA_H
#include "MatrixDataBase.h"
#include <vector>

template <typename T>
class FullMatrixData : public MatrixDataBase<T> {
public:
    FullMatrixData(int rows, int cols, MatrixFormType symmetryType) 
        : MatrixDataBase<T>(rows, cols, Full, symmetryType) {
        if (this->_symmetryType == NonSymmetric)
            this->_numElements = this->_rows * this->_cols;
        else {
            if (rows != cols)
                throw std::invalid_argument("Symmetric, UpperTriangular, and LowerTriangular matrices must have the same number of rows and columns.");
            this->_numElements = (rows * cols - rows) / 2 + rows;
        }
        this->_data = std::vector<T>(this->_numElements);
    }

protected:
    std::vector<T> _data;
};


#endif // FULLMATRIXDATA_H