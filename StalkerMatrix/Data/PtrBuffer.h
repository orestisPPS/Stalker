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
template <typename Derived, typename T>
class MatrixPtrBuffer {
public:
    // Constructor for non-const data
    MatrixPtrBuffer(T* data_start, std::size_t size)
        : _data_start(data_start), _size(size) {}

    // Constructor for const data
    MatrixPtrBuffer(const T* data_start, std::size_t size)
        : _data_start(const_cast<T*>(data_start)), _size(size) {}

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

    size_t size() const { return _size; }

    // Iterators for mutable access
    inline MatrixStrideIterator<T> begin() {
        return MatrixStrideIterator<T>(_data_start, derived().stride());
    }
    inline MatrixStrideIterator<T> end() {
        return MatrixStrideIterator<T>(_data_start + _size * derived().stride(), derived().stride());
    }

    // Iterators for const access
    inline MatrixStrideIterator<const T> cbegin() const {
        return MatrixStrideIterator<const T>(_data_start, derived().stride());
    }
    inline MatrixStrideIterator<const T> cend() const {
        return MatrixStrideIterator<const T>(_data_start + _size * derived().stride(), derived().stride());
    }

protected:
    T* _data_start;       ///< Pointer to the start of the buffer
    std::size_t _size;    ///< Number of elements in the buffer

private:
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

// =============================
// Contiguous Block Buffer
// =============================
template <typename T>
class ContiguousBlockPtrBuffer : public MatrixPtrBuffer<ContiguousBlockPtrBuffer<T>, T> {
public:
    // Constructor for non-const data
    ContiguousBlockPtrBuffer(T* data_start, std::size_t size)
        : MatrixPtrBuffer<ContiguousBlockPtrBuffer, T>(data_start, size) {}

    // Constructor for const data
    ContiguousBlockPtrBuffer(const T* data_start, std::size_t size)
        : MatrixPtrBuffer<ContiguousBlockPtrBuffer, T>(data_start, size) {}

    // Element access
    inline T& _access(std::size_t i) { return *(this->_data_start + i); }
    inline const T& _access(std::size_t i) const { return *(this->_data_start + i); }

    // Stride is always 1 for row-major row buffer
    inline std::ptrdiff_t stride() const { return 1; }
};

// =============================
// Stride Buffer
// =============================
template <typename T>
class StridePtrBuffer : public MatrixPtrBuffer<StridePtrBuffer<T>, T> {
public:
    // Constructor for non-const data
    StridePtrBuffer(T* data_start, std::size_t size, std::size_t n)
        : MatrixPtrBuffer<StridePtrBuffer, T>(data_start, size), _n(n) {}

    // Constructor for const data
    StridePtrBuffer(const T* data_start, std::size_t size, std::size_t n)
        : MatrixPtrBuffer<StridePtrBuffer, T>(data_start, size), _n(n) {}

    // Element access
    inline T& _access(std::size_t i) { return *(this->_data_start + _n - i); }
    inline const T& _access(std::size_t i) const { return *(this->_data_start + _n - i); }

    // n is determined by the number of rows
    inline std::ptrdiff_t stride() const { return 1; }

private:
    std::size_t _n; // Stride
};


#endif // MATRIX_PTRBUFFER_H
