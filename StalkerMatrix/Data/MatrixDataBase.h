#ifndef MATRIX_DATABASE_H
#define MATRIX_DATABASE_H

#include <vector>
#include <stdexcept>
#include <unordered_map>
#include "MatrixTypes.h"
// =============================
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

protected:
    size_t _rows; ///< Number of rows
    size_t _cols; ///< Number of columns

private:
    // Static cast to derived type
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

#endif // MATRIX_DATABASE_H
