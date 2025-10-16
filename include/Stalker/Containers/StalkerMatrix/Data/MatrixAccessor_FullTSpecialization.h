#ifndef MATRIXACCESSOR_TSPECIALIZATION_H
#define MATRIXACCESSOR_TSPECIALIZATION_H
#include "MatrixAccessorBase.h"

//=================================================================================================
// ======================== Full Dense Row Major Matrix Template Specialization ===================
//=================================================================================================
template <typename T, StorageLayout Layout, T_Form FormT, OrderType OrderT>
class MatrixAccessor : public MatrixAccessorBase<MatrixAccessor<T, Layout, FormT, OrderT>, T, Layout, FormT, OrderT> {

using Base = MatrixAccessorBase<MatrixAccessor<T, Layout, FormT, OrderT>, T, Layout, FormT, OrderT>;
using RowBuffer = MatrixPtrBuffer<T, Layout, FormT, OrderT, RegionType::Row>;
using ConstRowBuffer = MatrixPtrBuffer<const T, Layout, FormT, OrderT, RegionType::Row>;
using ColumnBuffer = MatrixPtrBuffer<T, Layout, FormT, OrderT, RegionType::Column>;
using ConstColumnBuffer = MatrixPtrBuffer<const T, Layout, FormT, OrderT, RegionType::Column>;

public:

    MatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * cols) {}
    MatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * cols, value) {}
    MatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize) {
            // checkSize(numValues, rows * cols, "MatrixAccessor:: Constructor");  
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
    inline RowBuffer _row(size_t row) {
        return RowBuffer(values.data() + row * this->_cols, this->_cols);
    }
    inline ConstRowBuffer _row(size_t row) const {
        return ConstRowBuffer(values.data() + row * this->_cols, this->_cols);
    }
    inline ColumnBuffer _column(size_t col) {
        return ColumnBuffer(values.data() + col, this->_rows, this->_rows);
    }
    inline ConstColumnBuffer _column(size_t col) const {
        return ConstColumnBuffer(values.data() + col, this->_rows, this->_rows);
    }
};

//=================================================================================================
// ======================== Full Dense Col Major Matrix Template Specialization ===================
//=================================================================================================
template <typename T>
class MatrixAccessor<T, StorageLayout::Dense, T_Form::Full, OrderType::ColumnMajor>
    : public MatrixAccessorBase<MatrixAccessor<T, StorageLayout::Dense, T_Form::Full, OrderType::ColumnMajor>, T, StorageLayout::Dense, T_Form::Full, OrderType::ColumnMajor> {
    
    using Base = MatrixAccessorBase<MatrixAccessor<T, StorageLayout::Dense, T_Form::Full, OrderType::ColumnMajor>, T, StorageLayout::Dense, T_Form::Full, OrderType::ColumnMajor>;
    using RowBuffer = MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::Full, OrderType::ColumnMajor, RegionType::Row>;
    using ConstRowBuffer = MatrixPtrBuffer<const T, StorageLayout::Dense, T_Form::Full, OrderType::ColumnMajor, RegionType::Row>;
    using ColumnBuffer = MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::Full, OrderType::ColumnMajor, RegionType::Column>;
    using ConstColumnBuffer = MatrixPtrBuffer<const T, StorageLayout::Dense, T_Form::Full, OrderType::ColumnMajor, RegionType::Column>;

public:

    MatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * cols) {}
    MatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * cols, value) {}
    MatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize)
            checkSize(numValues, rows * cols, "MatrixAccessor:: Constructor");  
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
    inline RowBuffer _row(size_t row) {
        return RowBuffer(values.data() + row, this->_cols, this->_cols);
    }
    inline ConstRowBuffer _row(size_t row) const {
        return ConstRowBuffer(values.data() + row, this->_cols, this->_cols);
    }
    inline ColumnBuffer _column(size_t col) {
        return ColumnBuffer(values.data() + col * this->_rows, this->_rows);
    }
    inline ConstColumnBuffer _column(size_t col) const {
        return ConstColumnBuffer(values.data() + col * this->_rows, this->_rows);
    }
};

#endif // MATRIXACCESSOR_TSPECIALIZATION_H