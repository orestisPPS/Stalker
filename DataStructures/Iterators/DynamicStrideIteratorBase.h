#ifndef DYNAMIC_STRIDE_ITERATOR_BASE_H
#define DYNAMIC_STRIDE_ITERATOR_BASE_H

#include "StrideIteratorBase.h"
/**
 * @brief Iterator for traversing blocks with dynamic stride.
 *
 * @tparam Derived The derived iterator class.
 * @tparam T The type of elements being iterated over.
 *
 * This CRTP-based iterator provides dynamic stride traversal, with the stride
 * determined by the derived class. It supports irregular memory layouts, making it
 * suitable for data structures like triangular matrices, sparse arrays, or
 * non-contiguous blocks.
 */
template <typename Derived, typename T>
class DynamicStrideIteratorBase : public StrideIteratorBase<Derived, T> {
    using Base = StrideIteratorBase<Derived, T>;

protected:
    friend Base;
    DynamicStrideIteratorBase(T* startPtr, std::ptrdiff_t stride) : Base(startPtr, stride), _index(0) {}
    DynamicStrideIteratorBase(const T* startPtr, std::ptrdiff_t stride) : Base(startPtr, stride), _index(0) {}

    inline void _increment() {
        this->_current += child()._strideFunction(_index); // Advance by the stride at the current index
        ++_index;
    }
    inline void _decrement() {
        --_index;
        this->_current -= child()._strideFunction(_index); // Retreat by the stride at the decremented index
    }
    // inline void _advance(std::ptrdiff_t n) {
    //     child()._advanceFunction(n);
    // }
        inline void _advance(std::ptrdiff_t n) {
        if (n > 0) {
            for (std::ptrdiff_t i = 0; i < n; ++i) {
                this->_increment();
            }
        } else {
            for (std::ptrdiff_t i = 0; i < -n; ++i) {
                this->_decrement();
            }
        }
    }

    inline std::ptrdiff_t _distance(const DynamicStrideIteratorBase& other) const {
        return static_cast<std::ptrdiff_t>(_index - other._index);
    }
    inline bool _equals(const DynamicStrideIteratorBase& other) const { return this->_current == other._current && _index == other._index; }
    inline bool _less_than(const DynamicStrideIteratorBase& other) const { return _index < other._index; }
    inline std::size_t index() const { return _index; }

private:
    std::size_t _index; ///< The current logical index.
    inline Derived& child() { return *static_cast<Derived*>(this); }
    inline const Derived& child() const { return *static_cast<const Derived*>(this); }
};

#endif // DYNAMIC_STRIDE_ITERATOR_BASE_H
