#ifndef INDEX_DEPENDENT_STRIDE_ITERATOR_H
#define INDEX_DEPENDENT_STRIDE_ITERATOR_H

template <typename T, bool Ascending = true>
class IndexDependentStrideIterator
  : public DynamicStrideIteratorBase<IndexDependentStrideIterator<T, Ascending>, T> {
    
    using Base = DynamicStrideIteratorBase<IndexDependentStrideIterator<T, Ascending>, T>;

public:
    // Constructor sets the "base offset" plus we default _stride=1 in the base
    IndexDependentStrideIterator(T* startPtr, std::ptrdiff_t baseOffset, std::size_t initialIndex = 0)
        : Base(startPtr, 1), _baseOffset(baseOffset) {
        this->_index = initialIndex;
    }

    IndexDependentStrideIterator(const T* startPtr, std::ptrdiff_t baseOffset, std::size_t initialIndex = 0)
        : Base(startPtr, 1), _baseOffset(baseOffset){
        this->_index = initialIndex;
    }

protected:
    friend Base;
    std::ptrdiff_t _baseOffset;

    inline std::ptrdiff_t _strideFunction(std::size_t currentIndex) const {
        if constexpr (Ascending)
            return _baseOffset + static_cast<std::ptrdiff_t>(currentIndex);
        else
            return _baseOffset - static_cast<std::ptrdiff_t>(currentIndex);
    }

    inline void _advanceFunction(typename Base::difference_type n) {
        for (std::ptrdiff_t i = 0; i < n; ++i)
            this->_increment();
    }
};

#endif // INDEX_DEPENDENT_STRIDE_ITERATOR_H