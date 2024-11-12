#ifndef MATRIX_DATA_H
#define MATRIX_DATA_H

#include <vector>
#include <stdexcept>
#include <unordered_map>
#include "MatrixTypes.h"
#include "PtrBuffer.h"// =============================
// Base MatrixDataBase Class
// =============================
template <typename Derived, typename T, FormType FormT, OrderType OrderT>
class MatrixDataBase {
public:
    MatrixDataBase(size_t rows, size_t cols) : _rows(rows), _cols(cols) {}

    inline size_t getNumRows() const { return _rows; }
    inline size_t getNumCols() const { return _cols; }

    // Public element access with bounds checking
    inline T& element(size_t i, size_t j) {
        if (i >= _rows || j >= _cols)
            throw std::out_of_range("Index out of bounds in Matrix Data.");
        return derived()._element(i, j);
    }

    inline const T& element(size_t i, size_t j) const {
        if (i >= _rows || j >= _cols)
            throw std::out_of_range("Index out of bounds in Matrix Data.");
        return derived()._element(i, j);
    }

    // Public row and column access
    inline auto row(size_t row) const {
        if (row >= _rows)
            throw std::out_of_range("Row index out of bounds in Matrix Data.");
        return derived()._row(row);
    }

    inline auto column(size_t col) const {
        if (col >= _cols)
            throw std::out_of_range("Column index out of bounds in Matrix Data.");
        return derived()._column(col);
    }

protected:
    size_t _rows; ///< Number of rows
    size_t _cols; ///< Number of columns

private:
    // Static cast to derived type
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

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

    // Row and column buffer creation
    inline auto _row(size_t row) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return RowBufferRowMajor<T>(values.data() + row * this->_cols, this->_cols);
        } else { // ColumnMajor
            return RowBufferColumnMajor<T>(values.data() + row, this->_cols, this->_rows);
        }
    }

    inline auto _column(size_t col) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return ColumnBufferRowMajor<T>(values.data() + col, this->_rows, this->_cols);
        } else { // ColumnMajor
            return ColumnBufferColumnMajor<T>(values.data() + col * this->_rows, this->_rows);
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
        if (i > j) std::swap(i, j);
        return values[i * (2 * this->_rows - i + 1) / 2 + j - i];
    }

    inline const T& _element(size_t i, size_t j) const {
        if (i > j) std::swap(i, j);
        return values[i * (2 * this->_rows - i + 1) / 2 + j - i];
    }

    // Row and column buffer creation for symmetric matrices
    inline auto _row(size_t row) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return RowBufferRowMajor<T>(values.data() + row * (row + 1) / 2, this->_cols);
        } else {
            return RowBufferColumnMajor<T>(values.data(), row + 1, this->_rows);
        }
    }

    inline auto _column(size_t col) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return ColumnBufferRowMajor<T>(values.data() + col * this->_rows, this->_rows, this->_cols);
        } else {
            return ColumnBufferColumnMajor<T>(values.data() + col * (col + 1) / 2, col + 1);
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

    // Row and column buffer creation for upper triangular matrices
    inline auto _row(size_t row) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return RowBufferRowMajor<T>(values.data() + row * (row + 1) / 2, this->_cols);
        } else {
            return RowBufferColumnMajor<T>(values.data(), row + 1, this->_rows);
        }
    }

    inline auto _column(size_t col) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return ColumnBufferRowMajor<T>(values.data(), col + 1, this->_cols);
        } else {
            return ColumnBufferColumnMajor<T>(values.data() + col * (col + 1) / 2, col + 1);
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

    // Row and column buffer creation for lower triangular matrices
    inline auto _row(size_t row) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return RowBufferRowMajor<T>(values.data(), row + 1);
        } else {
            return RowBufferColumnMajor<T>(values.data() + row * (row + 1) / 2, this->_cols, this->_rows);
        }
    }

    inline auto _column(size_t col) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return ColumnBufferRowMajor<T>(values.data() + col * this->_rows, this->_rows);
        } else {
            return ColumnBufferColumnMajor<T>(values.data() + col * (col + 1) / 2, col + 1);
        }
    }
};

#endif // MATRIX_DATA_H