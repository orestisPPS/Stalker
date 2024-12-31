#ifndef DENSEMATRIXACCESSOR_TRIANGULARTSPECIALIZATION_H
#define DENSEMATRIXACCESSOR_TRIANGULARTSPECIALIZATION_H


#include "DenseMatrixAccessor_FullTSpecialization.h"
//=================================================================================================
// ===================== Upper Triangular Row Major Matrix Template Specialization ================
//=================================================================================================
template <typename T>
class DenseMatrixAccessor : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::RowMajor>, T, FormType::UpperTriangular, OrderType::RowMajor> {

using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::RowMajor>, T, FormType::UpperTriangular, OrderType::RowMajor>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize) {
            checkSize(numValues, rows * cols, "DenseMatrixAccessor:: Constructor");  
        }
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend class Base;
    std::vector<T> values; ///< Flat array for symmetric matrix data

    inline T& _element(size_t i, size_t j) {
        return values[i * (2 * this->_rows - i + 1) / 2 + j - i];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[i * (2 * this->_rows - i + 1) / 2 + j - i];
    }
    inline auto _row(size_t row) {
        return GeneralMatrixPtrBuffer<T>(row * (2 * this-> - row + 1) / 2 - row, this->_rows - row);
    }
    inline auto _row(size_t row) const {
        return GeneralMatrixPtrBuffer<const T>(row * (2 * this->_rows - row + 1) / 2 - row, this->_rows - row);
    }
    inline auto _column(size_t col) {
    
    }
    inline auto _column(size_t col) const {
    
    }
};

//=================================================================================================
// ===================== Upper Triangular Column Major Matrix Template Specialization =============
//=================================================================================================

template <typename T>
class DenseMatrixAccessor : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::ColumnMajor>, T, FormType::UpperTriangular, OrderType::ColumnMajor> {

using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::ColumnMajor>, T, FormType::UpperTriangular, OrderType::ColumnMajor>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize)
            checkSize(numValues, rows * cols, "DenseMatrixAccessor:: Constructor");  
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend class Base;
    std::vector<T> values;

    inline T& _element(size_t i, size_t j) {
        return values[j * (2 * this->_rows - j + 1) / 2 + i - j];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[j * (2 * this->_rows - j + 1) / 2 + i - j];
    }
    inline auto _row(size_t row) {
    
    }
    inline auto _row(size_t row) const {
    
    }
    inline auto _column(size_t col) {
        return GeneralMatrixPtrBuffer<T>(col * (2 * this->_rows - col + 1) / 2 - col, col + 1);
    }
    inline auto _column(size_t col) const {
        return GeneralMatrixPtrBuffer<const T>(col * (2 * this->_rows - col + 1) / 2 - col, col + 1);
    }
};

//=================================================================================================
// ===================== Lower Triangular Row Major Matrix Template Specialization ================
//=================================================================================================

template <typename T>
class DenseMatrixAccessor : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::RowMajor>, T, FormType::LowerTriangular, OrderType::RowMajor> {

using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::RowMajor>, T, FormType::LowerTriangular, OrderType::RowMajor>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize) {
            checkSize(numValues, rows * cols, "DenseMatrixAccessor:: Constructor");  
        }
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend class Base;
    std::vector<T> values; ///< Flat array for symmetric matrix data

    inline T& _element(size_t i, size_t j) {
        return values[i * (2 * this->_rows - i + 1) / 2 + j - i];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[i * (2 * this->_rows - i + 1) / 2 + j - i];
    }
    inline auto _row(size_t row) {
        return GeneralMatrixPtrBuffer<T>(row * (2 * this-> - row + 1) / 2 - row, this->_rows - row);
    }
    inline auto _row(size_t row) const {
        return GeneralMatrixPtrBuffer<const T>(row * (2 * this->_rows - row + 1) / 2 - row, this->_rows - row);
    }
    inline auto _column(size_t col) {
    
    }
    inline auto _column(size_t col) const {
    
    }
};

//=================================================================================================
// ===================== Lower Triangular Column Major Matrix Template Specialization =============

template <typename T>
class DenseMatrixAccessor : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::ColumnMajor>, T, FormType::LowerTriangular, OrderType::ColumnMajor> {

using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::ColumnMajor>, T, FormType::LowerTriangular, OrderType::ColumnMajor>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize)
            checkSize(numValues, rows * cols, "DenseMatrixAccessor:: Constructor");  
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend class Base;
    std::vector<T> values;

    inline T& _element(size_t i, size_t j) {
        return values[j * (2 * this->_rows - j + 1) / 2 + i - j];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[j * (2 * this->_rows - j + 1) / 2 + i - j];
    }
    inline auto _row(size_t row) {
    
    }
    inline auto _row(size_t row) const {
    
    }
    inline auto _column(size_t col) {
        return GeneralMatrixPtrBuffer<T>(col * (2 * this->_rows - col + 1) / 2 - col, col + 1);
    }
    inline auto _column(size_t col) const {
        return GeneralMatrixPtrBuffer<const T>(col * (2 * this->_rows - col + 1) / 2 - col, col + 1);
    }
};


#endif // DENSEMATRIXACCESSOR_TRIANGULARTSPECIALIZATION_H