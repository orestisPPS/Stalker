#ifndef MATRIX_PTRBUFFER_H
#define MATRIX_PTRBUFFER_H

#include <cstddef>
#include <stdexcept>
#include "MatrixTypes.h"    
#include "StrideIterator.h"

enum class MatrixBufferType {
    Row,
    Column,
    Diagonal,
    Block
};
// =============================
// Base CRTP Class for Buffers
// =============================
template <typename Derived, typename T>
class MatrixPtrBuffer {
public:
    MatrixPtrBuffer(T* data_start, std::size_t size)
        : _data_start(data_start), _size(size) {}

    // Public element access with bounds checking
    inline T& operator[](std::size_t i) {
        if (i >= _size)
            throw std::out_of_range("MatrixPtrBuffer index out of range");
        return derived()._access(i); // Delegate to derived
    }

    inline const T& operator[](std::size_t i) const {
        if (i >= _size)
            throw std::out_of_range("MatrixPtrBuffer index out of range");
        return derived()._access(i); // Delegate to derived
    }

    // Iterators
    inline MatrixStrideIterator<T> begin() { return MatrixStrideIterator<T>(_data_start, derived().stride()); }
    inline MatrixStrideIterator<const T> cbegin() const { return MatrixStrideIterator<const T>(_data_start, derived().stride()); }
    inline MatrixStrideIterator<T> end() { return MatrixStrideIterator<T>(_data_start + _size * derived().stride(), derived().stride()); }
    inline MatrixStrideIterator<const T> cend() const { return MatrixStrideIterator<const T>(_data_start + _size * derived().stride(), derived().stride()); }

protected:
    T* _data_start;       ///< Pointer to the start of the buffer
    std::size_t _size;    ///< Number of elements in the buffer

private:
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

// =============================
// Row-Major Row Buffer
// =============================
template <typename T>
class RowBufferRowMajor : public MatrixPtrBuffer<RowBufferRowMajor<T>, T> {
public:
    RowBufferRowMajor(T* data_start, std::size_t size)
        : MatrixPtrBuffer<RowBufferRowMajor, T>(data_start, size) {}

    // Element access
    inline T& _access(std::size_t i) { return *(this->_data_start + i); }
    inline const T& _access(std::size_t i) const { return *(this->_data_start + i); }

    // Stride is always 1 for row-major row buffer
    inline std::ptrdiff_t stride() const { return 1; }
};

// =============================
// Column-Major Row Buffer
// =============================
template <typename T>
class RowBufferColumnMajor : public MatrixPtrBuffer<RowBufferColumnMajor<T>, T> {
public:
    RowBufferColumnMajor(T* data_start, std::size_t size, std::size_t numRows)
        : MatrixPtrBuffer<RowBufferColumnMajor, T>(data_start, size), _numRows(numRows) {}

    // Element access
    inline T& _access(std::size_t i) { return *(this->_data_start + i * _numRows); }
    inline const T& _access(std::size_t i) const { return *(this->_data_start + i * _numRows); }

    // Stride is determined by the number of rows
    inline std::ptrdiff_t stride() const { return _numRows; }

private:
    std::size_t _numRows; // Stride
};

// =============================
// Row-Major Column Buffer
// =============================
template <typename T>
class ColumnBufferRowMajor : public MatrixPtrBuffer<ColumnBufferRowMajor<T>, T> {
public:
    ColumnBufferRowMajor(T* data_start, std::size_t size, std::size_t numCols)
        : MatrixPtrBuffer<ColumnBufferRowMajor, T>(data_start, size), _numCols(numCols) {}

    // Element access
    inline T& _access(std::size_t i) { return *(this->_data_start + i * _numCols); }
    inline const T& _access(std::size_t i) const { return *(this->_data_start + i * _numCols); }

    // Stride is determined by the number of columns
    inline std::ptrdiff_t stride() const { return _numCols; }

private:
    std::size_t _numCols; // Stride
};

// =============================
// Column-Major Column Buffer
// =============================
template <typename T>
class ColumnBufferColumnMajor : public MatrixPtrBuffer<ColumnBufferColumnMajor<T>, T> {
public:
    ColumnBufferColumnMajor(T* data_start, std::size_t size)
        : MatrixPtrBuffer<ColumnBufferColumnMajor, T>(data_start, size) {}

    // Element access
    inline T& _access(std::size_t i) { return *(this->_data_start + i); }
    inline const T& _access(std::size_t i) const { return *(this->_data_start + i); }

    // Stride is always 1 for column-major column buffer
    inline std::ptrdiff_t stride() const { return 1; }
};
#endif // MATRIX_PTRBUFFER_H