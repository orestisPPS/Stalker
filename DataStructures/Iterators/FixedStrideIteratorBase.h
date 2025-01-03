#ifndef FIXED_STRIDE_ITERATOR_BASE_H
#define FIXED_STRIDE_ITERATOR_BASE_H

#include "StrideIteratorBase.h"

template <typename Derived, typename T>
class FixedStrideIteratorBase : public StrideIteratorBase<Derived, T> {
    using Base = StrideIteratorBase<Derived, T>;

protected:
    friend Base;

   template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    FixedStrideIteratorBase(U* startPtr, std::ptrdiff_t stride) : Base(startPtr, stride) {}

    inline constexpr void _increment() { this->_current += child()._stride(); }
    inline constexpr void _decrement() { this->_current -= child()._stride(); }
    inline constexpr void _advance(typename Base::difference_type n) { this->_current += n * child()._stride(); }
    inline constexpr typename Base::difference_type _distance(const FixedStrideIteratorBase& other) const {
        return (this->_current - other._current) / child()._stride();
    }
    inline constexpr bool _equals(const FixedStrideIteratorBase& other) const { return this->_current == other._current; }
    inline constexpr bool _less_than(const FixedStrideIteratorBase& other) const { return this->_current < other._current; }

    
private:
    inline Derived& child() { return *static_cast<Derived*>(this); }
    inline const Derived& child() const { return *static_cast<const Derived*>(this); }
};


template <typename T>
class FixedStrideIterator : public FixedStrideIteratorBase<FixedStrideIterator<T>, T> {
    using Base = FixedStrideIteratorBase<FixedStrideIterator<T>, T>;

public:
    FixedStrideIterator(T* startPtr, std::ptrdiff_t stride) : Base(startPtr, stride) {}
    
protected:
    friend Base;
    inline constexpr std::ptrdiff_t _stride() const { return Base::_stride; }
};

#endif // FIXED_STRIDE_ITERATOR_BASE_H