#ifndef DENSEMATRIXDATAACCESSOR_TSPECIALIZATION_H
#define DENSEMATRIXDATAACCESSOR_TSPECIALIZATION_H
#include "MatrixAccessorBase.h"

//=================================================================================================
// ======================== Full Dense Row Major Matrix Template Specialization ===================
//=================================================================================================
template <typename T, FormType FormT, OrderType OrderT>
class DenseMatrixAccessor : public MatrixAccessorBase<DenseMatrixAccessor<T, FormT, OrderT>, T, FormT, OrderT> {

using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormT, OrderT>, T, FormT, OrderT>;
using Buffer = DenseMatrixGenericPtrBuffer<T>;
using ConstBuffer = DenseMatrixGenericPtrBuffer<const T>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * cols, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize) {
            // checkSize(numValues, rows * cols, "DenseMatrixAccessor:: Constructor");  
        }
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values; ///< Flat array for symmetric matrix data

    inline T& _element(size_t i, size_t j) {
        return values[i * this->_cols + j];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[i * this->_cols + j];
    }
    inline Buffer _row(size_t row) {
        return Buffer(values.data() + row * this->_cols, this->_cols);
    }
    inline ConstBuffer _row(size_t row) const {
        return ConstBuffer(values.data() + row * this->_cols, this->_cols);
    }
    inline Buffer _column(size_t col) {
        return Buffer(values.data() + col, this->_rows, this->_rows);
    }
    inline ConstBuffer _column(size_t col) const {
        return ConstBuffer(values.data() + col, this->_rows, this->_rows);
    }
};

//=================================================================================================
// ======================== Full Dense Col Major Matrix Template Specialization ===================
//=================================================================================================
template <typename T>
class DenseMatrixAccessor<T, FormType::Full, OrderType::ColumnMajor>
    : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::Full, OrderType::ColumnMajor>, T, FormType::Full, OrderType::ColumnMajor> {
    
    using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::Full, OrderType::ColumnMajor>, T, FormType::Full, OrderType::ColumnMajor>;
    using Buffer = DenseMatrixGenericPtrBuffer<T>;
    using ConstBuffer = DenseMatrixGenericPtrBuffer<const T>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize)
            checkSize(numValues, rows * cols, "DenseMatrixAccessor:: Constructor");  
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values;

    inline T& _element(size_t i, size_t j) {
        return values[j * this->_rows + i];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[j * this->_rows + i];
    }
    inline Buffer _row(size_t row) {
        return Buffer(values.data() + row, this->_cols, this->_cols);
    }
    inline ConstBuffer _row(size_t row) const {
        return ConstBuffer(values.data() + row, this->_cols, this->_cols);
    }
    inline Buffer _column(size_t col) {
        return Buffer(values.data() + col * this->_rows, this->_rows);
    }
    inline ConstBuffer _column(size_t col) const {
        return ConstBuffer(values.data() + col * this->_rows, this->_rows);
    }
};

#endif // DENSEMATRIXDATAACCESSOR_TSPECIALIZATION_H