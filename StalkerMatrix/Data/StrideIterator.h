#ifndef MATRIX_STRIDE_ITERATOR_H
#define MATRIX_STRIDE_ITERATOR_H

#include <iterator>
#include <stdexcept>
#include <cstddef>
#include <iterator>
#include <cstddef> // For std::ptrdiff_t
#include "../MatrixTypes.h"

/**
 * @brief A random-access iterator for traversing matrix elements with a fixed stride.
 * 
 * @tparam T The type of elements being iterated over.
 */
template <typename T>
class StrideIterator {

protected:
    T* _current;       ///< Pointer to the current element.
    std::ptrdiff_t _stride; ///< Stride between elements.

public:
    // STL-compliant iterator traits
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    StrideIterator(T* current, std::ptrdiff_t stride) : _current(current), _stride(stride) {}

    inline reference operator*() const { return *_current; }

    inline pointer operator->() const { return _current; }

    inline StrideIterator& operator++() {
        _current += _stride;
        return *this;
    }

    inline StrideIterator operator++(int) {
        StrideIterator temp = *this;
        ++(*this);
        return temp;
    }

    inline StrideIterator& operator--() {
        _current -= _stride;
        return *this;
    }

    inline StrideIterator operator--(int) {
        StrideIterator temp = *this;
        --(*this);
        return temp;
    }

    inline StrideIterator operator+(difference_type n) const {
        return StrideIterator(_current + n * _stride, _stride);
    }

    inline StrideIterator& operator+=(difference_type n) {
        _current += n * _stride;
        return *this;
    }

    inline StrideIterator operator-(difference_type n) const {
        return StrideIterator(_current - n * _stride, _stride);
    }

    inline StrideIterator& operator-=(difference_type n) {
        _current -= n * _stride;
        return *this;
    }

    inline difference_type operator-(const StrideIterator& other) const {
        return (_current - other._current) / _stride;
    }

    inline reference operator[](difference_type n) const {
        return *(_current + n * _stride);
    }

    // Comparison operators
    inline bool operator==(const StrideIterator& other) const { return _current == other._current; }
    inline bool operator!=(const StrideIterator& other) const { return !(*this == other); }
    inline bool operator<(const StrideIterator& other) const { return _current < other._current; }
    inline bool operator>(const StrideIterator& other) const { return _current > other._current; }
    inline bool operator<=(const StrideIterator& other) const { return !(*this > other); }
    inline bool operator>=(const StrideIterator& other) const { return !(*this < other); }
};

#endif // MATRIX_STRIDE_ITERATOR_H
