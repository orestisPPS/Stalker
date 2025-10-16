#pragma once

#include <Stalker/Containers/Iterators/DynamicStrideIteratorBase.h>


namespace Stalker::Containers::Iterators {
template <typename T, bool Ascending>
class LinearStrideIterator : public DynamicStrideIteratorBase<LinearStrideIterator<T, Ascending>, T> {
    using Base = DynamicStrideIteratorBase<LinearStrideIterator<T, Ascending>, T>;

public:                                                                                                
    LinearStrideIterator(T* startPtr, std::ptrdiff_t startStride, size_t index = 0) : Base(startPtr, 1), _startStride(startStride) {
        this->_index = index;
    }

protected:
    friend Base;
    std::ptrdiff_t _startStride; ///< Initial stride value.

    inline std::ptrdiff_t _strideFunction(size_t index) const {
        if constexpr (Ascending)
            return _startStride + index * this->_stride;
        else
            return _startStride - index * this->_stride; 
    }
};

} // namespace Stalker::Containers::Iterators
