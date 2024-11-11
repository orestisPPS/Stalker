#ifndef MATRIXBUFFERS_H
#define MATRIXBUFFERS_H

#include <cstddef>
#include <stdexcept>
#include "MatrixTypes.h"    
#include "MatrixIterators.h"

template <typename T, MatrixElementsOrder orderType>
class MatrixRowBuffer {
private:
    T* _data_start;       ///< Pointer to the start of the matrix data.
    std::size_t _size;    ///< Number of elements in the row.
    std::size_t _numRows; ///< Total number of rows in the matrix.

public:
    MatrixRowBuffer(T* data_start, std::size_t size, std::size_t numRows)
        : _data_start(data_start), _size(size), _numRows(numRows) {}

    // Access element with bounds checking
    inline T& operator[](std::size_t i) {
        if (i >= _size) {
            throw std::out_of_range("MatrixRowBuffer index out of range");
        }
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            return *(_data_start + i);
        } else if constexpr (orderType == MatrixElementsOrder::ColumnMajor) {
            return *(_data_start + i * _numRows);
        }
    }

    inline const T& operator[](std::size_t i) const {
        if (i >= _size) {
            throw std::out_of_range("MatrixRowBuffer index out of range");
        }
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            return *(_data_start + i);
        } else if constexpr (orderType == MatrixElementsOrder::ColumnMajor) {
            return *(_data_start + i * _numRows);
        }
    }

    inline std::size_t size() const { return _size; }

    inline T* data() { return _data_start; }
    inline const T* data() const { return _data_start; }

    inline MatrixStrideIterator<T> begin() { return _beginIterator<T>(); }
    inline MatrixStrideIterator<const T> cbegin() const { return _beginIterator<const T>(); }
    inline MatrixStrideIterator<T> end() { return _endIterator<T>(); }
    inline MatrixStrideIterator<const T> cend() const { return _endIterator<const T>(); }

private:
    template <typename S>
    inline MatrixStrideIterator<S> _beginIterator() const {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            return MatrixStrideIterator<S>(_data_start, 1);
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return MatrixStrideIterator<S>(_data_start, _numRows);
    }

    template <typename S>
    inline MatrixStrideIterator<S> _endIterator() const {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            return MatrixStrideIterator<S>(_data_start + _size, 1);
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return MatrixStrideIterator<S>(_data_start + _size * _numRows, _numRows);
    }
};

template <typename T, MatrixElementsOrder orderType>
class MatrixColumnBuffer {
private:
    T* _data_start;        ///< Pointer to the start of the matrix data.
    std::size_t _size;     ///< Number of elements in the column.
    std::size_t _numCols;  ///< Total number of columns in the matrix.

public:
    MatrixColumnBuffer(T* data_start, std::size_t size, std::size_t numCols)
        : _data_start(data_start), _size(size), _numCols(numCols) {
            if (!data_start)
                throw std::invalid_argument("Buffer initiated with null pointer");
    }

    inline T& operator[](std::size_t i) {
        if (i >= _size) { throw std::out_of_range("MatrixColumnBuffer index out of range"); }
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            return *(_data_start + i * _numCols);
        } else if constexpr (orderType == MatrixElementsOrder::ColumnMajor) {
            return *(_data_start + i);
        }
    }

    inline const T& operator[](std::size_t i) const {
        if (i >= _size) { throw std::out_of_range("MatrixColumnBuffer index out of range"); }
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            return *(_data_start + i * _numCols);
        } else if constexpr (orderType == MatrixElementsOrder::ColumnMajor) {
            return *(_data_start + i);
        }
    }

    inline std::size_t size() const { return _size; }

    inline T* data() { return _data_start; }
    inline const T* data() const { return _data_start; }

    inline MatrixStrideIterator<T> begin() { return _beginIterator<T>(); }
    inline MatrixStrideIterator<const T> cbegin() const { return _beginIterator<const T>(); }
    inline MatrixStrideIterator<T> end() { return _endIterator<T>(); }
    inline MatrixStrideIterator<const T> cend() const { return _endIterator<const T>(); }

private:
    template <typename S>
    inline MatrixStrideIterator<S> _beginIterator() const {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            return MatrixStrideIterator<S>(_data_start, _numCols);
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return MatrixStrideIterator<S>(_data_start, 1);
    }

    template <typename S>
    inline MatrixStrideIterator<S> _endIterator() const {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            return MatrixStrideIterator<S>(_data_start + _size * _numCols, _numCols);
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return MatrixStrideIterator<S>(_data_start + _size, 1);
    }
};


#endif // MATRIXBUFFERS_H