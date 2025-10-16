#ifndef STALKER_MATRIX_H
#define STALKER_MATRIX_H
#include <type_traits>
#include "MatrixTypes.h"
#include "Data/MatrixAccessor_FullTSpecialization.h"
#include "Data/MatrixAccessor_SymmetricTSpecialization.h"
#include "Data/MatrixAccessor_TriangularTSpecialization.h"
// #include "Data/MatrixAccessor_COOTSpecialization.h"
    
template<typename T, StorageLayout Layout = StorageLayout::Dense, T_Form FormT = T_Form::Full, OrderType OrderT = OrderType::RowMajor>       
class StalkerMatrix {
   
public:
    StalkerMatrix() : _rows(0), _columns(0), _matrixAccessor(0, 0) {}
    StalkerMatrix(unsigned rows, unsigned columns) : _rows(rows), _columns(columns) {}
    StalkerMatrix(unsigned rows, unsigned columns, const T& value) : _rows(rows), _columns(columns), _matrixAccessor(rows, columns, value) {}
    StalkerMatrix(unsigned rows, unsigned columns, const T* values, size_t numValues) : _rows(rows), _columns(columns), _matrixAccessor(rows, columns, values, numValues) {}
    // Destructor
    ~StalkerMatrix() = default;
    // Copy Constructor
    StalkerMatrix(const StalkerMatrix& other) : _rows(other._rows), _columns(other._columns), _matrixAccessor(other._matrixAccessor) {}
    // Move Constructor
    StalkerMatrix(StalkerMatrix&& other) noexcept :
        _rows(other._rows), _columns(other._columns), _matrixAccessor(std::move(other._matrixAccessor)) {
        other._rows = 0;
        other._columns = 0;
    }
    // Copy Assignment Operator
    StalkerMatrix& operator=(const StalkerMatrix& other) {
        if (this != &other) {
            _rows = other._rows;
            _columns = other._columns;
            _matrixAccessor = other._matrixAccessor;
        }
        return *this;
    }
    // Move Assignment Operator
    StalkerMatrix& operator=(StalkerMatrix&& other) noexcept {
        if (this != &other) {
            _rows = other._rows;
            _columns = other._columns;
            _matrixAccessor = std::move(other._matrixAccessor);

            other._rows = 0;
            other._columns = 0;
        }
        return *this;
    }

    inline T& operator()(size_t row, size_t col) {
        return _matrixAccessor.element(row, col);
    }

    inline const T& operator()(size_t row, size_t col) const {
        return _matrixAccessor.element(row, col);
    }

    auto getRow(size_t row) {
        return _matrixAccessor.row(row);
    }

    auto getRow(size_t row) const {
        return _matrixAccessor.row(row);
    }

    auto getColumn(size_t col) {
        return _matrixAccessor.column(col);
    }

    auto getColumn(size_t col) const {
        return _matrixAccessor.column(col);
    }
    
    unsigned getNumRows() const { return _rows; }
    unsigned getNumColumns() const { return _columns; }

protected:
    unsigned _rows;
    unsigned _columns;
    MatrixAccessor<T, Layout, FormT, OrderT> _matrixAccessor;

};
#endif // STALKER_MATRIX_H