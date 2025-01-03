#ifndef FIXED_STRIDE_PTR_BUFFER_H
#define FIXED_STRIDE_PTR_BUFFER_H

#include "PtrBufferBase.h"
#include "../Iterators/FixedStrideIteratorBase.h"

template <typename T>
class FixedStridePtrBuffer : public PtrBufferBase<T, FixedStridePtrBuffer<T>> {
public:

    FixedStridePtrBuffer(T* data_start, std::size_t size, std::ptrdiff_t stride = 1)
        : PtrBufferBase<T, FixedStridePtrBuffer<T>>(data_start, size, stride) {}

    FixedStridePtrBuffer(const T* data_start, std::size_t size, std::ptrdiff_t stride = 1)
        : PtrBufferBase<T, FixedStridePtrBuffer<T>>(const_cast<T*>(data_start), size, stride) {}


    inline T& _at(std::size_t i) {
        return *(this->_data_start + i * this->_stride);
    }
    inline const T& _at(std::size_t i) const {
        return *(this->_data_start + i * this->_stride);
    }
    inline FixedStrideIterator<T> _begin() {
        return FixedStrideIterator<T>(this->_data_start, this->_stride);
    }
    inline FixedStrideIterator<T> _end() {
        return FixedStrideIterator<T>(this->_data_start + this->_size * this->_stride, this->_stride);
    }
    inline FixedStrideIterator<const T> _cbegin() const {
        return FixedStrideIterator<const T>(this->_data_start, this->_stride);
    }
    inline FixedStrideIterator<const T> _cend() const {
        return FixedStrideIterator<const T>(this->_data_start + this->_size * this->_stride, this->_stride);
    }
};



#endif // FIXED_STRIDE_PTR_BUFFER_H