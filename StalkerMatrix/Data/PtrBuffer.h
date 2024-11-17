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
template <typename T, typename ChildPtrBuffer>
class PtrBufferBase {
public:
    PtrBufferBase(T* data_start, std::size_t size, std::ptrdiff_t stride = 1) :
        _data_start(data_start), _size(size), _stride(stride) {}

    PtrBufferBase(const T* data_start, std::size_t size, std::ptrdiff_t stride = 1) :
        _data_start(const_cast<T*>(data_start)), _size(size), _stride(stride) {}


    inline T& operator[](std::size_t i) {
        if (i >= _size)
            throw std::out_of_range("MatrixPtrBuffer index out of range");
        return child()[i];
    }
    inline const T& operator[](std::size_t i) const {
        if (i >= _size)
            throw std::out_of_range("MatrixPtrBuffer index out of range");
        return child()[i];
    }

    size_t size() const { return _size; }
    inline auto begin() { return child().begin(); }
    inline auto end() { return child().end(); }
    inline auto cbegin() const { return child().cbegin(); }
    inline auto cend() const { return child().cend(); }

protected:
    T* _data_start;       ///< Pointer to the start of the buffer
    std::size_t _size;    ///< Number of elements in the buffer
    std::ptrdiff_t _stride; ///< Stride between elements
    inline ChildPtrBuffer& child() { return static_cast<ChildPtrBuffer&>(*this); }
    inline const ChildPtrBuffer& child() const { return static_cast<const ChildPtrBuffer&>(*this); }
};

// =============================
// Contiguous Block Ptr Buffer
// =============================

template <typename T>
class ContiguousBlockPtrBuffer : public PtrBufferBase<T, ContiguousBlockPtrBuffer<T>> {
public:
    ContiguousBlockPtrBuffer(T* data_start, std::size_t size, std::ptrdiff_t stride = 1)
        : PtrBufferBase<T, ContiguousBlockPtrBuffer<T>>(data_start, size, stride) {}

    inline T& operator[](std::size_t i) { 
        return *(this->_data_start + i * this->_stride); 
    }

    inline const T& operator[](std::size_t i) const { 
        return *(this->_data_start + i * this->_stride); 
    }

    inline T& at(std::size_t i) {
        if (i >= this->_size)
        throw std::out_of_range("MatrixPtrBuffer index out of range");
        return *(this->_data_start + i * this->_stride); 
    }

    inline const T& at(std::size_t i) const {
        if (i >= this->_size)
        throw std::out_of_range("MatrixPtrBuffer index out of range");
        return *(this->_data_start + i * this->_stride); 
    }

    inline ContiguousBlockIterator<T> begin() {
        return ContiguousBlockIterator<T>(this->_data_start, this->_stride);
    }

    inline ContiguousBlockIterator<T> end() {
        return ContiguousBlockIterator<T>(this->_data_start + this->_size * this->_stride, this->_stride);
    }

    inline ContiguousBlockIterator<const T> cbegin() const {
        return ContiguousBlockIterator<const T>(this->_data_start, this->_stride);
    }

    inline ContiguousBlockIterator<const T> cend() const {
        return ContiguousBlockIterator<const T>(this->_data_start + this->_size * this->_stride, this->_stride);
    }
};


// =============================
// Non-Contiguous Block Ptr Buffer
// =============================

template <typename T>
class NonContiguousBlockPtrBuffer : public PtrBufferBase<T, NonContiguousBlockPtrBuffer<T>> {
private:
    std::size_t _primaryDim; ///< Total number of rows (or columns) in the primary dimension.
    std::size_t _secondaryIdx; ///< Current secondary index (e.g., column index in row-major).

public:
    NonContiguousBlockPtrBuffer(T* data_start, std::size_t size, std::size_t primaryDim, std::ptrdiff_t stride, std::size_t secondaryIdx = 0)
        : PtrBufferBase<T, NonContiguousBlockPtrBuffer<T>>(data_start, size, stride),
          _primaryDim(primaryDim), _secondaryIdx(secondaryIdx) {}

    inline T& operator[](std::size_t i) {
        return *(this->_data_start + i * this->_stride); 
    }

    inline const T& operator[](std::size_t i) const {
        return *(NonContiguousBlockIterator<T>(this->_data_start, this->_stride, _primaryDim, 0, _secondaryIdx) + i); 
    }

    inline T& at(std::size_t i) {
        if (i >= this->_size)
        throw std::out_of_range("MatrixPtrBuffer index out of range");
        return *(this->_data_start + i * this->_stride); 
    }

    inline const T& at(std::size_t i) const {
        if (i >= this->_size)
        throw std::out_of_range("MatrixPtrBuffer index out of range");
        return *(NonContiguousBlockIterator<T>(this->_data_start, this->_stride, _primaryDim, 0, _secondaryIdx) + i); 
    }

    inline NonContiguousBlockIterator<T> begin() {
        return NonContiguousBlockIterator<T>(this->_data_start, this->_stride, _primaryDim, 0, _secondaryIdx);
    }

    inline NonContiguousBlockIterator<T> end() {
        return NonContiguousBlockIterator<T>(this->_data_start, this->_stride, _primaryDim, this->_size, _secondaryIdx);
    }

    inline NonContiguousBlockIterator<const T> cbegin() const {
        return NonContiguousBlockIterator<const T>(this->_data_start, this->_stride, _primaryDim, 0, _secondaryIdx);
    }

    inline NonContiguousBlockIterator<const T> cend() const {
        return NonContiguousBlockIterator<const T>(this->_data_start, this->_stride, _primaryDim, this->_size, _secondaryIdx);
    }
};


#endif // MATRIX_PTRBUFFER_H
