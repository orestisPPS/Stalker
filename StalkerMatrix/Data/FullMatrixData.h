#ifndef MATRIX_FULLMATRIXDATA_H
#define MATRIX_FULLMATRIXDATA_H

#include <vector>
#include <stdexcept>
#include <unordered_map>
#include "MatrixTypes.h"
#include "MatrixDataBase.h"


// ==============================
// Dense Matrix: Non-Symmetric
// ==============================
template <typename T, FormType FormT, OrderType OrderT>
class DenseMatrixData : public MatrixDataBase<DenseMatrixData<T, FormT, OrderT>, T, FormT, OrderT> {
public:
    DenseMatrixData(size_t rows, size_t cols)
        : MatrixDataBase<DenseMatrixData, T, FormT, OrderT>(rows, cols), values(rows * cols) {}

    DenseMatrixData(size_t rows, size_t cols, const T& value)
        : MatrixDataBase<DenseMatrixData, T, FormT, OrderT>(rows, cols), values(rows * cols, value) {}

    std::vector<T> values; ///< Flat array to store matrix elements

private:
    // Element access implementation
    inline T& _element(size_t i, size_t j) {
        if constexpr (OrderT == OrderType::RowMajor) {
            return values[i * this->_cols + j];
        } else { // ColumnMajor
            return values[j * this->_rows + i];
        }
    }

    inline const T& _element(size_t i, size_t j) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return values[i * this->_cols + j];
        } else { // ColumnMajor
            return values[j * this->_rows + i];
        }
    }
};

// ==============================
// Dense Matrix: Symmetric
// ==============================
template <typename T, OrderType OrderT>
class DenseMatrixData<T, FormType::Symmetric, OrderT>
    : public MatrixDataBase<DenseMatrixData<T, FormType::Symmetric, OrderT>, T, FormType::Symmetric, OrderT> {
public:
    DenseMatrixData(size_t rows, size_t cols)
        : MatrixDataBase<DenseMatrixData, T, FormType::Symmetric, OrderT>(rows, cols),
          values(rows * (rows + 1) / 2) {}

    DenseMatrixData(size_t rows, size_t cols, const T& value)
        : MatrixDataBase<DenseMatrixData, T, FormType::Symmetric, OrderT>(rows, cols),
          values(rows * (rows + 1) / 2, value) {}

    std::vector<T> values; ///< Flat array for symmetric matrix data

private:
    // Element access implementation for symmetric matrices
    inline T& _element(size_t i, size_t j) {
        if constexpr (OrderT == OrderType::RowMajor) {
            return (i <= j) ? values[i * this->_cols + j] : values[j * this->_cols + i];
        } else { // ColumnMajor
            return (i <= j) ? values[j * this->_rows + i] : values[i * this->_rows + j];
        }
    }

    inline const T& _element(size_t i, size_t j) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return (i <= j) ? values[i * this->_cols + j] : values[j * this->_cols + i];
        } else { // ColumnMajor
            return (i <= j) ? values[j * this->_rows + i] : values[i * this->_rows + j];
        }
    }
};

// ==============================
// Dense Matrix: Upper Triangular
// ==============================
template <typename T, OrderType OrderT>
class DenseMatrixData<T, FormType::UpperTriangular, OrderT>
    : public MatrixDataBase<DenseMatrixData<T, FormType::UpperTriangular, OrderT>, T, FormType::UpperTriangular, OrderT> {
public:
    DenseMatrixData(size_t rows, size_t cols)
        : MatrixDataBase<DenseMatrixData, T, FormType::UpperTriangular, OrderT>(rows, cols),
          values(rows * (rows + 1) / 2) {}

    DenseMatrixData(size_t rows, size_t cols, const T& value)
        : MatrixDataBase<DenseMatrixData, T, FormType::UpperTriangular, OrderT>(rows, cols),
          values(rows * (rows + 1) / 2, value) {}

    std::vector<T> values; ///< Flat array for upper triangular matrix data

private:
    // Element access implementation for upper triangular matrices
    inline T& _element(size_t i, size_t j) {
        if constexpr (OrderT == OrderType::RowMajor) {
            return (i <= j) ? values[i * this->_cols + j] : values[j * this->_cols + i];
        } else { // ColumnMajor
            return (i <= j) ? values[j * this->_rows + i] : values[i * this->_rows + j];
        }
    }

    inline const T& _element(size_t i, size_t j) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return (i <= j) ? values[i * this->_cols + j] : values[j * this->_cols + i];
        } else { // ColumnMajor
            return (i <= j) ? values[j * this->_rows + i] : values[i * this->_rows + j];
        }
    }
};

// ==============================
// Dense Matrix: Lower Triangular
// ==============================
template <typename T, OrderType OrderT>
class DenseMatrixData<T, FormType::LowerTriangular, OrderT>
    : public MatrixDataBase<DenseMatrixData<T, FormType::LowerTriangular, OrderT>, T, FormType::LowerTriangular, OrderT> {
public:
    DenseMatrixData(size_t rows, size_t cols)
        : MatrixDataBase<DenseMatrixData, T, FormType::LowerTriangular, OrderT>(rows, cols),
          values(rows * (rows + 1) / 2) {}

    DenseMatrixData(size_t rows, size_t cols, const T& value)
        : MatrixDataBase<DenseMatrixData, T, FormType::LowerTriangular, OrderT>(rows, cols),
          values(rows * (rows + 1) / 2, value) {}

    std::vector<T> values; ///< Flat array for lower triangular matrix data
    
private:
    // Element access implementation for lower triangular matrices
    inline T& _element(size_t i, size_t j) {
        if constexpr (OrderT == OrderType::RowMajor) {
            return (i >= j) ? values[i * this->_cols + j] : values[j * this->_cols + i];
        } else { // ColumnMajor
            return (i >= j) ? values[j * this->_rows + i] : values[i * this->_rows + j];
        }
    }

    inline const T& _element(size_t i, size_t j) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return (i >= j) ? values[i * this->_cols + j] : values[j * this->_cols + i];
        } else { // ColumnMajor
            return (i >= j) ? values[j * this->_rows + i] : values[i * this->_rows + j];
        }
    }
};

#endif // MATRIX_FULLMATRIXDATA_H