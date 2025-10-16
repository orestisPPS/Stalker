#pragma once

#include <Stalker/Containers/Iterators/FixedStrideIteratorBase.h>

template <typename T>
class FixedStrideIterator : public FixedStrideIteratorBase<FixedStrideIterator<T>, T> {
    using Base = FixedStrideIteratorBase<FixedStrideIterator<T>, T>;

public:
    FixedStrideIterator(T* startPtr, std::ptrdiff_t stride) : Base(startPtr, stride) {}
    
protected:
    friend Base;
    inline constexpr std::ptrdiff_t _stride() const {
        return Base::_stride;
    }
};