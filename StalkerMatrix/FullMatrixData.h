#ifndef FULLMATRIXDATA_H
#define FULLMATRIXDATA_H
#include "MatrixDataBase.h"
#include <vector>

template <typename T>
class FullMatrixData : public MatrixDataBase<T> {
public:
    FullMatrixData(int rows, int cols, MatrixSymmetryType symmetryType) 
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

template <typename T>
class RowMajorNonSymmetricFullMatrixAccessor {
public:
    NonSymmetricFullMatrixAccessor(FullMatrixData<T>& data) : _data(data) {}
    inline T& operator()(int row, int col) {
        return _data._data[row * _data._cols + col];
    }

    inline T operator()(int row, int col) const {
        return _data._data[row * _data._cols + col];
    }

    inline T operator()(int row, int col) const {
        return _data._data[row * _data._cols + col];
    }
};

template <typename T>
class RowMajorSymmetricFullMatrixAccessor {
public:
    RowMajorSymmetricFullMatrixAccessor(FullMatrixData<T>& data) : _data(data) {}
    inline T& operator()(int row, int col) {
        if (row < col)
            return _data._data[col * (col + 1) / 2 + row];
        else
            return _data._data[row * (row + 1) / 2 + col];
    }

    inline T operator()(int row, int col) const {
        if (row < col)
            return _data._data[col * (col + 1) / 2 + row];
        else
            return _data._data[row * (row + 1) / 2 + col];
    }

    inline T operator()(int row, int col) const {
        if (row < col)
            return _data._data[col * (col + 1) / 2 + row];
        else
            return _data._data[row * (row + 1) / 2 + col];
    }
};

#endif // FULLMATRIXDATA_H