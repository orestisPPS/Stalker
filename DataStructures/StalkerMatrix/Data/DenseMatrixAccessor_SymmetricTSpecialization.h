#ifndef MATRIXACCESSOR_SYMMETRICTSPECIALIZATION_H
#define MATRIXACCESSOR_SYMMETRICTSPECIALIZATION_H

#include "DenseMatrixAccessor_FullTSpecialization.h"

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
    inline GeneralMatrixPtrBuffer<T> _row(size_t row) {
        return GeneralMatrixPtrBuffer<T>(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<const T> _row(size_t row) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<T> _column(size_t col) {
        return GeneralMatrixPtrBuffer<T>(values.data() + col, col + 1);
    }
    inline GeneralMatrixPtrBuffer<const T> _column(size_t col) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + col, col + 1);
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
    inline GeneralMatrixPtrBuffer<T> _row(size_t row) {
        return GeneralMatrixPtrBuffer<T>(values.data() + (row * (row + 1)) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<const T> _row(size_t row) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + (row * (row + 1)) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<T> _column(size_t col) {
        return GeneralMatrixPtrBuffer<T>(values.data() + (col * (col + 1)) / 2, col + 1);
    }
    inline GeneralMatrixPtrBuffer<const T> _column(size_t col) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + (col * (col + 1)) / 2, col + 1);
    }
};

#endif // MATRIXACCESSOR_SYMMETRICTSPECIALIZATION_H