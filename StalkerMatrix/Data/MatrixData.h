#ifndef MATRIX_DATA_H
#define MATRIX_DATA_H

#include <vector>
#include <stdexcept>
#include <unordered_map>
#include "PtrBuffer.h"

// =============================
// Base MatrixDataBase Class
// =============================
template <typename Derived, typename T, FormType FormT, OrderType OrderT>
class MatrixDataBase {
public:
    MatrixDataBase(size_t rows, size_t cols) : _rows(rows), _cols(cols) {}

    static constexpr FormType FormTypeValue = FormT;   
    static constexpr OrderType OrderTypeValue = OrderT; // Expose OrderType
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

    DenseMatrixData(size_t rows, size_t cols, const T* values, size_t numValues) 
        : MatrixDataBase<DenseMatrixData, T, FormT, OrderT>(rows, cols) {
            if (numValues != rows * cols)
                throw std::invalid_argument("Invalid number of values for dense matrix.");
            this->values = std::vector<T>(values, values + numValues);
        }

    std::vector<T> values; ///< Flat array to store matrix elements

protected:
    friend class MatrixDataBase<DenseMatrixData<T, FormT, OrderT>, T, FormT, OrderT>;
    // Element access implementation
    inline T& _element(size_t i, size_t j) {
        if constexpr (OrderT == OrderType::RowMajor)
            return values[i * this->_cols + j];
        else if constexpr (OrderT == OrderType::ColumnMajor)
            return values[j * this->_rows + i];
    }

    inline const T& _element(size_t i, size_t j) const {
        if constexpr (OrderT == OrderType::RowMajor)
            return values[i * this->_cols + j];
        else if constexpr (OrderT == OrderType::ColumnMajor)
            return values[j * this->_rows + i];
    }

    // Row and column buffer creation
    inline auto _row(size_t row) const {
        if constexpr (OrderT == OrderType::RowMajor)
            return RowBufferRowMajor<T>(values.data() + row * this->_cols, this->_cols);
        else if constexpr (OrderT == OrderType::ColumnMajor)
            return RowBufferColumnMajor<T>(values.data() + row, this->_cols, this->_rows);
    }

    inline auto _column(size_t col) const {
        if constexpr (OrderT == OrderType::RowMajor)
            return ColumnBufferRowMajor<T>(values.data() + col, this->_rows, this->_cols);
        else if constexpr (OrderT == OrderType::ColumnMajor)
            return ColumnBufferColumnMajor<T>(values.data() + col * this->_rows, this->_rows);
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

    DenseMatrixData(size_t rows, size_t cols, const T* values, size_t numValues)
        : MatrixDataBase<DenseMatrixData, T, FormType::Symmetric, OrderT>(rows, cols) {
            if (numValues != rows * (rows + 1) / 2)
                throw std::invalid_argument("Invalid number of values for symmetric matrix.");
            this->values = std::vector<T>(values, values + numValues);
        }

    std::vector<T> values; ///< Flat array for symmetric matrix data

protected:
    friend class MatrixDataBase<DenseMatrixData<T, FormType::Symmetric, OrderT>, T, FormType::Symmetric, OrderT>;
    inline T& _element(size_t i, size_t j) {
        if (i > j) std::swap(i, j);
        if constexpr (OrderT == OrderType::RowMajor)
            return values[(i * (2 * this->_rows - i + 1)) / 2];
        else if constexpr (OrderT == OrderType::ColumnMajor)
            return values[(j * (2 * this->_rows - j + 1)) / 2 + (i - j)];
    }

    inline const T& _element(size_t i, size_t j) const {
        if (i > j) std::swap(i, j);
        if constexpr (OrderT == OrderType::RowMajor)
            return values[(i * (2 * this->_rows - i + 1)) / 2];
        else if constexpr (OrderT == OrderType::ColumnMajor)
            return values[(j * (2 * this->_rows - j + 1)) / 2 + (i - j)];
    }

    inline auto _row(size_t row) const {
        if constexpr (OrderT == OrderType::RowMajor)
            return RowBufferRowMajor<T>(values.data() + (row * (2 * this->_rows - row + 1)) / 2, this->_cols - row);
        else if constexpr (OrderT == OrderType::ColumnMajor)
            return RowBufferColumnMajor<T>(values.data() + (row * (row + 1)) / 2, this->_cols - row, this->_rows);
    }

    inline auto _column(size_t col) const {
        if constexpr (OrderT == OrderType::RowMajor) {
            return ColumnBufferRowMajor<T>(values.data() + col, col + 1, this->_cols);
        } else if constexpr (OrderT == OrderType::ColumnMajor) {
            return ColumnBufferColumnMajor<T>(values.data() + (col * (col + 1)) / 2, col + 1);
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

    DenseMatrixData(size_t rows, size_t cols, const T* values, size_t numValues)
        : MatrixDataBase<DenseMatrixData, T, FormType::UpperTriangular, OrderT>(rows, cols) {
            if (numValues != rows * (rows + 1) / 2)
                throw std::invalid_argument("Invalid number of values for upper triangular matrix.");
            this->values = std::vector<T>(values, values + numValues);
        }

    std::vector<T> values; ///< Flat array for upper triangular matrix data

protected:
    friend class MatrixDataBase<DenseMatrixData<T, FormType::UpperTriangular, OrderT>, T, FormType::UpperTriangular, OrderT>;
    inline T& _element(size_t i, size_t j) {
        if (i > j) throw std::out_of_range("Accessing non-stored element in upper triangular matrix.");
        return values[(i * (2 * this->_rows - i + 1)) / 2];
    }

    inline const T& _element(size_t i, size_t j) const {
        if (i > j) throw std::out_of_range("Accessing non-stored element in upper triangular matrix.");
        return values[(i * (2 * this->_rows - i + 1)) / 2];
    }

    inline auto _row(size_t row) const {
        if constexpr (OrderT == OrderType::RowMajor)
            return RowBufferRowMajor<T>(values.data() + (row * (2 * this->_rows - row + 1)) / 2, this->_cols - row);
        else if constexpr (OrderT == OrderType::ColumnMajor)
            return RowBufferColumnMajor<T>(values.data() + (row * (row + 1)) / 2, this->_cols - row, this->_rows);
    }

    inline auto _column(size_t col) const {
        if constexpr (OrderT == OrderType::RowMajor)
            return ColumnBufferRowMajor<T>(values.data() + (col * (2 * this->_rows - col + 1)) / 2, col + 1, this->_cols);
        else if constexpr (OrderT == OrderType::ColumnMajor)
            return ColumnBufferColumnMajor<T>(values.data() + (col * (col + 1)) / 2, col + 1);
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

    DenseMatrixData(size_t rows, size_t cols, const T* values, size_t numValues)
        : MatrixDataBase<DenseMatrixData, T, FormType::LowerTriangular, OrderT>(rows, cols){
            if (numValues != rows * (rows + 1) / 2)
                throw std::invalid_argument("Invalid number of values for lower triangular matrix.");
                this->values = std::vector<T>(values, values + numValues);
        }


    std::vector<T> values; ///< Flat array for lower triangular matrix data

protected:
    friend class MatrixDataBase<DenseMatrixData<T, FormType::LowerTriangular, OrderT>, T, FormType::LowerTriangular, OrderT>;
    inline T& _element(size_t i, size_t j) {
        if (i < j) throw std::out_of_range("Accessing non-stored element in lower triangular matrix.");
        return values[(j * (2 * this->_rows - j + 1)) / 2 + (i - j)];
    }

    inline const T& _element(size_t i, size_t j) const {
        if (i < j) throw std::out_of_range("Accessing non-stored element in lower triangular matrix.");
        return values[(j * (2 * this->_rows - j + 1)) / 2 + (i - j)];
    }

    inline auto _row(size_t row) const {
        if constexpr (OrderT == OrderType::RowMajor)
            return RowBufferRowMajor<T>(values.data() + (row * (row + 1)) / 2, row + 1);
        else if constexpr (OrderT == OrderType::ColumnMajor)
            return RowBufferColumnMajor<T>(values.data() + (row * (2 * this->_rows - row + 1)) / 2, this->_cols - row, this->_rows);
    }

    inline auto _column(size_t col) const {
        if constexpr (OrderT == OrderType::RowMajor)
            return ColumnBufferRowMajor<T>(values.data() + (col * (2 * this->_rows - col + 1)) / 2, this->_rows - col, this->_cols);
        else if constexpr (OrderT == OrderType::ColumnMajor)
            return ColumnBufferColumnMajor<T>(values.data() + (col * (col + 1)) / 2, col + 1);
    }
};

#endif // MATRIX_DATA_H
