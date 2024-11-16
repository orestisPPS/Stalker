#ifndef MATRIX_PTRBUFFER_H
#define MATRIX_PTRBUFFER_H

#include <cstddef>
#include <stdexcept>
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
template <typename T>
class StridePtrBuffer {
public:
    StridePtrBuffer(T* data_start, std::size_t size) :
        _data_start(data_start), _size(size), _stride(1) {}

    StridePtrBuffer(const T* data_start, std::size_t size) :
        _data_start(const_cast<T*>(data_start)), _size(size), _stride(1) {}

    StridePtrBuffer(T* data_start, std::size_t size, std::ptrdiff_t stride) :
        _data_start(data_start), _size(size), _stride(stride) {}

    StridePtrBuffer(const T* data_start, std::size_t size, std::ptrdiff_t stride) :
        _data_start(const_cast<T*>(data_start)), _size(size), _stride(stride) {}

    // Public element access with bounds checking
    inline T& operator[](std::size_t i) {
        if (i >= _size)
            throw std::out_of_range("MatrixPtrBuffer index out of range");
        return *(this->_data_start + i * _stride);
    }
    inline const T& operator[](std::size_t i) const {
        if (i >= _size)
            throw std::out_of_range("MatrixPtrBuffer index out of range");
        return *(this->_data_start + i *  _stride);
    }

    size_t size() const { return _size; }

    // Iterators for mutable access
    inline StrideIterator<T> begin() {
        return StrideIterator<T>(_data_start, _stride);
    }
    inline StrideIterator<T> end() {
        return StrideIterator<T>(_data_start + _size * _stride, _stride);
    }

    // Iterators for const access
    inline StrideIterator<const T> cbegin() const {
        return StrideIterator<const T>(_data_start, _stride);
    }
    inline StrideIterator<const T> cend() const {
        return StrideIterator<const T>(_data_start + _size * _stride, _stride);
    }

protected:
    T* _data_start;       ///< Pointer to the start of the buffer
    std::size_t _size;    ///< Number of elements in the buffer
    std::ptrdiff_t _stride; ///< Stride between elements
};



#endif // MATRIX_PTRBUFFER_H
