#ifndef MATRIXACCESSOR_SYMMETRICTSPECIALIZATION_H
#define MATRIXACCESSOR_SYMMETRICTSPECIALIZATION_H

#include "DenseMatrixAccessor_FullTSpecialization.h"
#include "../Buffers/MatrixPtrBuffer.h"

template <typename T>
class DenseMatrixAccessor<T, FormType::Symmetric, OrderType::RowMajor>
    : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::Symmetric, OrderType::RowMajor>, T, FormType::Symmetric, OrderType::RowMajor> {

    using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::Symmetric, OrderType::RowMajor>, T, FormType::Symmetric, OrderType::RowMajor>;
    using RowBuffer = MatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::RowMajor, RegionType::Row>;
    using ConstRowBuffer = MatrixPtrBuffer<const T, FormType::UpperTriangular, OrderType::RowMajor, RegionType::Row>;
    using ColumnBuffer = MatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::RowMajor, RegionType::Column>;
    using ConstColumnBuffer = MatrixPtrBuffer<const T, FormType::UpperTriangular, OrderType::RowMajor, RegionType::Column>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize) {
            checkSize(numValues, rows * (rows + 1) / 2, "DenseMatrixAccessor:: Constructor");  
        }
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values; ///< Flat array for symmetric matrix data

    inline T& _element(size_t i, size_t j) {
        if (i > j)
            std::swap(i, j);
        return values[i * (2 * this->_rows - i + 1) / 2 + (j - i)];
    }
    inline const T& _element(size_t i, size_t j) const {
        if (i > j)
            std::swap(i, j);
        return values[i * (2 * this->_rows - i + 1) / 2 + (j - i)];
    }
    inline RowBuffer _row(size_t row) {
        return RowBuffer(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row); 
    }
    inline ConstRowBuffer _row(size_t row) const {
        return ConstRowBuffer(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row);
    }
    inline ColumnBuffer _column(size_t col) {
        return ColumnBuffer(values.data(), col + 1, col, this->_rows);
    }
    inline ConstColumnBuffer _column(size_t col) const {
        return ConstColumnBuffer(values.data(), col + 1, col, this->_rows);
    }
};

template <typename T>
class DenseMatrixAccessor<T, FormType::Symmetric, OrderType::ColumnMajor>
    : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::Symmetric, OrderType::ColumnMajor>, T, FormType::Symmetric, OrderType::ColumnMajor> {

    using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::Symmetric, OrderType::ColumnMajor>, T, FormType::Symmetric, OrderType::ColumnMajor>;
    using RowBuffer = MatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>;
    using ConstRowBuffer = MatrixPtrBuffer<const T, FormType::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>;
    using ColumnBuffer = MatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor, RegionType::Column>;
    using ConstColumnBuffer = MatrixPtrBuffer<const T, FormType::UpperTriangular, OrderType::ColumnMajor, RegionType::Column>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize)
            checkSize(numValues, rows * (rows + 1) / 2, "DenseMatrixAccessor:: Constructor");  
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values;

    inline T& _element(size_t i, size_t j) {
        if (i > j)
            std::swap(i, j);
        return values[j * (j + 1) / 2 + i];
    }
    inline const T& _element(size_t i, size_t j) const {
        if (i > j)
            std::swap(i, j);
        return values[j * (j + 1) / 2 + i];
    }
    inline RowBuffer _row(size_t row) {
        return RowBuffer(values.data(), this->_rows - row, row, this->_rows);
    }
    inline ConstRowBuffer _row(size_t row) const {
        return ConstRowBuffer(values.data(), this->_rows - row, row, this->_rows);
    }
    inline ColumnBuffer _column(size_t col) {
        return ColumnBuffer(values.data() + col * (col + 1) / 2, col + 1);
    }
    inline ConstColumnBuffer _column(size_t col) const {
        return ConstColumnBuffer(values.data() + col * (col + 1) / 2, col + 1);
    }
};

#endif // MATRIXACCESSOR_SYMMETRICTSPECIALIZATION_H