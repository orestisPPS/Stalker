#ifndef MATRIXACCESSOR_SYMMETRICTSPECIALIZATION_H
#define MATRIXACCESSOR_SYMMETRICTSPECIALIZATION_H

#include "DenseMatrixAccessor_FullTSpecialization.h"
#include "../Buffers/DenseTriangularMatrixPtrBuffer.h"

template <typename T>
class DenseMatrixAccessor<T, FormType::Symmetric, OrderType::RowMajor>
    : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::Symmetric, OrderType::RowMajor>, T, FormType::Symmetric, OrderType::RowMajor> {

    using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::Symmetric, OrderType::RowMajor>, T, FormType::Symmetric, OrderType::RowMajor>;

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
    inline DenseMatrixGenericPtrBuffer<T> _row(size_t row) {
        return DenseMatrixGenericPtrBuffer<T>(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row);
    }
    inline DenseMatrixGenericPtrBuffer<const T> _row(size_t row) const {
        return DenseMatrixGenericPtrBuffer<const T>(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row);
    }
    inline DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::RowMajor> _column(size_t col) {
        return DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::RowMajor>(
            values.data() + col * (2 * this->_rows - col + 1) / 2, col + 1, this->_rows);
    }
    inline DenseTriangularMatrixPtrBuffer<const T, FormType::UpperTriangular, OrderType::RowMajor> _column(size_t col) const {
        return DenseTriangularMatrixPtrBuffer<const T, FormType::UpperTriangular, OrderType::RowMajor>(
            values.data() + col * (2 * this->_rows - col + 1) / 2, col + 1, this->_rows);
    }
};
template <typename T>
class DenseMatrixAccessor<T, FormType::Symmetric, OrderType::ColumnMajor>
    : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::Symmetric, OrderType::ColumnMajor>, T, FormType::Symmetric, OrderType::ColumnMajor> {

    using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::Symmetric, OrderType::ColumnMajor>, T, FormType::Symmetric, OrderType::ColumnMajor>;
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
        return values[j * (2 * this->_rows - j + 1) / 2 + i - j];
    }
    inline const T& _element(size_t i, size_t j) const {
        if (i > j)
            std::swap(i, j);
        return values[j * (2 * this->_rows - j + 1) / 2 + i - j];
    }
    inline DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor> _row(size_t row) {
        return DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor>(
            values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row, this->_rows);
    }
    inline DenseTriangularMatrixPtrBuffer<const T, FormType::Symmetric, OrderType::ColumnMajor> _row(size_t row) const {
        return DenseTriangularMatrixPtrBuffer<const T, FormType::Symmetric, OrderType::ColumnMajor>(
            values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row, this->_rows);
    }
    inline DenseMatrixGenericPtrBuffer<T> _column(size_t col) {
        return DenseMatrixGenericPtrBuffer<T>(values.data() + (col * (col + 1)) / 2, col + 1);
    }
    inline DenseMatrixGenericPtrBuffer<const T> _column(size_t col) const {
        return DenseMatrixGenericPtrBuffer<const T>(values.data() + (col * (col + 1)) / 2, col + 1);
    }
};

#endif // MATRIXACCESSOR_SYMMETRICTSPECIALIZATION_H