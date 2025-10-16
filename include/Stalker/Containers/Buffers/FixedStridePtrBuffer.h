#pragma once

#include "PtrBufferBase.h"
#include "../Iterators/FixedStrideIteratorBase.h"

namespace Stalker::Containers::Buffers {

    template <typename T>
    class FixedStridePtrBuffer : public PtrBufferBase<T, FixedStridePtrBuffer<T>> {
        using Base = PtrBufferBase<T, FixedStridePtrBuffer<T>>;
    public:

        FixedStridePtrBuffer(T* dataStart, std::size_t size, std::ptrdiff_t stride = 1) : Base(dataStart, size, stride) {}

        FixedStridePtrBuffer(const T* dataStart, std::size_t size, std::ptrdiff_t stride = 1) : Base(const_cast<T*>(dataStart), size, stride) {}
    protected:
        friend Base;

        inline T& _at(std::size_t i) {
            return *(this->_dataPtr + i * this->_stride);
        }
        inline const T& _at(std::size_t i) const {
            return *(this->_dataPtr + i * this->_stride);
        }
        inline FixedStrideIterator<T> _begin() {
            return FixedStrideIterator<T>(this->_dataPtr, this->_stride);
        }
        inline FixedStrideIterator<T> _end() {
            return FixedStrideIterator<T>(this->_dataPtr + this->_size * this->_stride, this->_stride);
        }
        inline FixedStrideIterator<const T> _cbegin() const {
            return FixedStrideIterator<const T>(this->_dataPtr, this->_stride);
        }
        inline FixedStrideIterator<const T> _cend() const {
            return FixedStrideIterator<const T>(this->_dataPtr + this->_size * this->_stride, this->_stride);
        }
    };

} // namespace Stalker::Containers::Buffers
