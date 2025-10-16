#pragma once

#include <Stalker/Containers/Iterators/StrideIteratorBase.h>

template <typename Derived, typename T>
class FixedStrideIteratorBase : public StrideIteratorBase<Derived, T> {
    using Base = StrideIteratorBase<Derived, T>;

protected:
    friend Base;

    FixedStrideIteratorBase(T* startPtr, std::ptrdiff_t stride) : Base(startPtr, stride) {}

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
