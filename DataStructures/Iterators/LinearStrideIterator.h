#ifndef LINEAR_STRIDE_ITERATOR_H
#define LINEAR_STRIDE_ITERATOR_H

#include "DynamicStrideIteratorBase.h"

template <typename T, bool Ascending>
class LinearStrideIterator : public DynamicStrideIteratorBase<LinearStrideIterator<T, Ascending>, T> {
    using Base = DynamicStrideIteratorBase<LinearStrideIterator<T, Ascending>, T>;

public:                                                                                                
    LinearStrideIterator(T* startPtr, std::ptrdiff_t startStride, size_t index = 0) : Base(startPtr, 1), _startStride(startStride) {
        this->_index = index;
    }
    LinearStrideIterator(const T* startPtr, std::ptrdiff_t startStride, size_t index = 0) : Base(startPtr, 1), _startStride(startStride) {
        this->_index = index;
    }

protected:
    friend Base;
    std::ptrdiff_t _startStride; ///< Initial stride value.

    inline std::ptrdiff_t _strideFunction(size_t index) const { 
        return _startStride + index * this->_stride; 
    }

    inline void _advanceFunction(typename Base::difference_type n) {
        for (std::ptrdiff_t i = 0; i < n; ++i)
            this->_increment();
    }
};

template <typename T>
class LinearStrideIterator<T, false> : public DynamicStrideIteratorBase<LinearStrideIterator<T, false>, T> {
    using Base = DynamicStrideIteratorBase<LinearStrideIterator<T, false>, T>;

public:
    LinearStrideIterator(T* startPtr, std::ptrdiff_t startStride, size_t index = 0) : Base(startPtr, 1), _startStride(startStride) {
        this->_index = index;
    }
    LinearStrideIterator(const T* startPtr, std::ptrdiff_t startStride, size_t index = 0) : Base(startPtr, 1), _startStride(startStride) {
        this->_index = index;
    }

protected:
    friend Base;
    std::ptrdiff_t _startStride; ///< Initial stride value.

    inline std::ptrdiff_t _strideFunction(size_t index) const { 
        return _startStride - index * this->_stride; 
    }

    inline void _advanceFunction(typename Base::difference_type n) {
        for (std::ptrdiff_t i = 0; i < n; ++i)
            this->_increment();
    }
};

#endif // LINEAR_STRIDE_ITERATOR_H