#ifndef MATRIX_DATA_H
#define MATRIX_DATA_H

#include <vector>
#include "../MatrixTypes.h"
#include "../Buffers/DenseMatrixGenericPtrBuffer.h"

// =============================
// Base MatrixDataBase Class
// =============================
template <typename Derived, typename T, FormType Form, OrderType Order>
class MatrixAccessorBase {
public:
    MatrixAccessorBase(size_t rows, size_t cols) : _rows(rows), _cols(cols) {}

    static constexpr FormType FormT = Form;   
    static constexpr OrderType OrderT = Order;
    inline size_t getNumRows() const { return _rows; }
    inline size_t getNumCols() const { return _cols; }

    inline T& element(size_t i, size_t j) {
        if constexpr (RawDoggyMatrixIndeces)
            checkMatrixIndeces(i, j, _rows, _cols, "MatrixAccessorBase::element");
        return derived()._element(i, j);
    }

    inline const T& element(size_t i, size_t j) const {
        if constexpr (RawDoggyMatrixIndeces)
            checkMatrixIndeces(i, j, _rows, _cols, "MatrixAccessorBase::element");
        return derived()._element(i, j);
    }

    // Public row and column access
    inline auto row(size_t row) {
        if constexpr (RawDoggyIndex)
            checkIndex(row, _rows, "MatrixAccessorBase::row");
        return derived()._row(row);
    }

    inline const auto row(size_t row) const {
        if constexpr (RawDoggyIndex)
            checkIndex(row, _rows, "MatrixAccessorBase::row");
        return derived()._row(row);
    }

    inline auto column(size_t col) {
        if constexpr (RawDoggyIndex)
            checkIndex(col, _cols, "MatrixAccessorBase::column");
        return derived()._column(col);
    }

    inline const auto column(size_t col) const {
        if constexpr (RawDoggyIndex)
            checkIndex(col, _cols, "MatrixAccessorBase::column");
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


#endif // MATRIX_DATA_H
