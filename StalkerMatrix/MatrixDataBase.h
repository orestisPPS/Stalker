#ifndef MATRIXDATABASE_H
#define MATRIXDATABASE_H

#include <stdexcept>
#include "MatrixDataAccessors.h"
#include "MatrixTypes.h"

template <typename T>
class MatrixDataBase {

public:
    // Default constructor
    MatrixDataBase() : _rows(0), _cols(0), _size(0), _numElements(0),
                    _storageType(MatrixStorageType::None),
                    _form(MatrixFormType::None),
                    _order(MatrixElementsOrder::None) {}

    // Parameterized constructor
    MatrixDataBase(unsigned int rows, unsigned int cols, MatrixStorageType storageType, MatrixFormType formType, MatrixElementsOrder order) : 
        _rows(rows), _cols(cols), _size(rows * cols), _numElements(0), _storageType(storageType), _form(formType), _order(order) {}

    // Copy constructor
    MatrixDataBase(const MatrixDataBase& other) : 
        _rows(other._rows), _cols(other._cols), _size(other._size), _numElements(other._numElements), 
        _storageType(other._storageType), _form(other._form), _order(other._order), _accessor(other._accessor) {}
    // Move constructor
    MatrixDataBase(MatrixDataBase&& other) noexcept : 
        _rows(other._rows), _cols(other._cols), _size(other._size), _numElements(other._numElements), 
        _storageType(other._storageType), _form(other._form), _order(other._order), _accessor(std::move(other._accessor)) {
        other._rows = 0;
        other._cols = 0;
        other._size = 0;
        other._numElements = 0;
    }

    // Copy assignment operator
    MatrixDataBase& operator=(const MatrixDataBase& other) {
        if (this != &other) {
            _rows = other._rows;
            _cols = other._cols;
            _size = other._size;
            _numElements = other._numElements;
            _storageType = other._storageType;
            _form = other._form;
            _order = other._order;
            _accessor = other._accessor;
        }
        return *this;
    }

    // Move assignment operator
    MatrixDataBase& operator=(MatrixDataBase&& other) noexcept {
        if (this != &other) {
            _rows = other._rows;
            _cols = other._cols;
            _size = other._size;
            _numElements = other._numElements;
            _storageType = other._storageType;
            _form = other._form;
            _order = other._order;
            _accessor = std::move(other._accessor);
            other._rows = 0;
            other._cols = 0;
            other._size = 0;
            other._numElements = 0;
        }
        return *this;
    }

    // Destructor
    virtual ~MatrixDataBase() = default;

    // Member functions
    unsigned getSize() const { return _size; }
    unsigned getNumElements() const { return _numElements; }
    MatrixStorageType getStorageType() const { return _storageType; }
    MatrixFormType getSymmetryType() const { return _form; }
    MatrixElementsOrder getElementsOrder() const { return _order; }
    unsigned getRowCount() const { return _rows; }
    unsigned getColCount() const { return _cols; }
    virtual T getElement(unsigned row, unsigned col) const = 0;
    virtual void setElement(unsigned row, unsigned col, T value) = 0;

    virtual void getRow(unsigned row, T* start, T* end = nullptr) const = 0;


    // Overloaded operators
    T operator()(unsigned row, unsigned col) const {
        return _accessor()
    }

    T& operator()(unsigned row, unsigned col) {
        static T temp;
        setElement(row, col, temp);
        return temp;
    }

protected:

    unsigned int _rows;
    unsigned int _cols;
    unsigned int _size;
    unsigned int _numElements;
    MatrixStorageType _storageType;
    MatrixFormType _form;
    MatrixElementsOrder _order;
    MatrixAccessor<T, > _accessor;

    inline void _checkBounds(int row, int col) const {
        if (row < 0 || row >= _rows)
            throw std::out_of_range("Row index out of bounds");
        if (col < 0 || col >= _cols)
            throw std::out_of_range("Column index out of bounds");
    }
};

#endif // MATRIXDATABASE_H