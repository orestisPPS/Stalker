#ifndef LINEAR_STRIDE_ITERATOR_H
#define LINEAR_STRIDE_ITERATOR_H

#include "DynamicStrideIteratorBase.h"

template <typename T, bool Ascending>
class LinearStrideIterator : public DynamicStrideIteratorBase<LinearStrideIterator<T, Ascending>, T> {
    using Base = DynamicStrideIteratorBase<LinearStrideIterator<T, Ascending>, T>;

public:                                                                                                
    LinearStrideIterator(T* startPtr, std::ptrdiff_t start_stride, std::ptrdiff_t stride = 1)
        : Base(startPtr, stride), _start_stride(start_stride) {}

protected:
    friend Base;
    std::ptrdiff_t _start_stride; ///< Initial stride value.

    inline std::ptrdiff_t _strideFunction(std::size_t index) const { 
        return _start_stride + index * this->_stride; 
    }

    inline void _advanceFunction(typename Base::difference_type n) {
        for (std::ptrdiff_t i = 0; i < n; ++i) {
            this->_increment();
        }
    }
};

template <typename T>
class LinearStrideIterator<T, false> : public DynamicStrideIteratorBase<LinearStrideIterator<T, false>, T> {
    using Base = DynamicStrideIteratorBase<LinearStrideIterator<T, false>, T>;

public:
    LinearStrideIterator(T* startPtr, std::ptrdiff_t start_stride, std::ptrdiff_t stride = 1)
        : Base(startPtr, stride), _start_stride(start_stride) {}

protected:
    friend Base;
    std::ptrdiff_t _start_stride; ///< Initial stride value.

    inline std::ptrdiff_t _strideFunction(std::size_t index) const { 
        return _start_stride - index * this->_stride; 
    }

    inline void _advanceFunction(typename Base::difference_type n) {
        for (std::ptrdiff_t i = 0; i < n; ++i) {
            this->_increment();
        }
    }
};

#endif // LINEAR_STRIDE_ITERATOR_H