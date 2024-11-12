#ifndef MATRIX_STRIDE_ITERATOR_H
#define MATRIX_STRIDE_ITERATOR_H

#include <iterator>
#include <stdexcept>
#include <cstddef>
#include <iterator>
#include <cstddef> // For std::ptrdiff_t
#include "MatrixTypes.h"

/**
 * @brief A random-access iterator for traversing matrix elements with a fixed stride.
 * 
 * @tparam T The type of elements being iterated over.
 */
template <typename T>
class MatrixStrideIterator {
private:
    T* _current;       ///< Pointer to the current element.
    unsigned _stride;  ///< Stride between consecutive elements.

public:
    // STL-compliant iterator traits
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    /**
     * @brief Constructs a MatrixStrideIterator.
     * 
     * @param current Pointer to the starting element.
     * @param stride Stride to the next element in the sequence.
     */
    MatrixStrideIterator(T* current, unsigned stride)
        : _current(current), _stride(stride) {}

    /**
     * @brief Dereference the iterator to access the current element.
     * 
     * @return A reference to the element at the current position.
     */
    inline reference operator*() const { return *_current; }

    /**
     * @brief Access members of the current element (for pointer-like behavior).
     * 
     * @return A pointer to the current element.
     */
    inline pointer operator->() const { return _current; }

    /**
     * @brief Pre-increment the iterator.
     * 
     * Advances the iterator to the next element in the sequence.
     * 
     * @return A reference to the incremented iterator.
     */
    inline MatrixStrideIterator& operator++() {
        _current += _stride;
        return *this;
    }

    /**
     * @brief Post-increment the iterator.
     * 
     * Advances the iterator to the next element in the sequence, but returns a copy of the iterator before the increment.
     * 
     * @return A copy of the iterator before it was incremented.
     */
    inline MatrixStrideIterator operator++(int) {
        MatrixStrideIterator temp = *this;
        ++(*this);
        return temp;
    }

    /**
     * @brief Pre-decrement the iterator.
     * 
     * Moves the iterator to the previous element in the sequence.
     * 
     * @return A reference to the decremented iterator.
     */
    inline MatrixStrideIterator& operator--() {
        _current -= _stride;
        return *this;
    }

    /**
     * @brief Post-decrement the iterator.
     * 
     * Moves the iterator to the previous element in the sequence, but returns a copy of the iterator before the decrement.
     * 
     * @return A copy of the iterator before it was decremented.
     */
    inline MatrixStrideIterator operator--(int) {
        MatrixStrideIterator temp = *this;
        --(*this);
        return temp;
    }

    /**
     * @brief Advances the iterator by a given number of elements.
     * 
     * @param n The number of elements to advance the iterator.
     * @return A new iterator advanced by n elements.
     */
    inline MatrixStrideIterator operator+(difference_type n) const {
        return MatrixStrideIterator(_current + n * _stride, _stride);
    }

    /**
     * @brief Advances the iterator by a given number of elements (in-place).
     * 
     * @param n The number of elements to advance the iterator.
     * @return A reference to the advanced iterator.
     */
    inline MatrixStrideIterator& operator+=(difference_type n) {
        _current += n * _stride;
        return *this;
    }

    /**
     * @brief Moves the iterator backwards by a given number of elements.
     * 
     * @param n The number of elements to move backwards.
     * @return A new iterator moved backwards by n elements.
     */
    inline MatrixStrideIterator operator-(difference_type n) const {
        return MatrixStrideIterator(_current - n * _stride, _stride);
    }

    /**
     * @brief Moves the iterator backwards by a given number of elements (in-place).
     * 
     * @param n The number of elements to move backwards.
     * @return A reference to the moved iterator.
     */
    inline MatrixStrideIterator& operator-=(difference_type n) {
        _current -= n * _stride;
        return *this;
    }

    /**
     * @brief Computes the distance between two iterators.
     * 
     * @param other The iterator to compare against.
     * @return The distance between the two iterators in number of elements.
     */
    inline difference_type operator-(const MatrixStrideIterator& other) const {
        return (_current - other._current) / _stride;
    }

    /**
     * @brief Access an element at an arbitrary offset from the current iterator.
     * 
     * @param n The offset (in number of elements).
     * @return A reference to the element at the offset.
     */
    inline reference operator[](difference_type n) const {
        return *(_current + n * _stride);
    }

    // Comparison operators
    inline bool operator==(const MatrixStrideIterator& other) const { return _current == other._current; }
    inline bool operator!=(const MatrixStrideIterator& other) const { return !(*this == other); }
    inline bool operator<(const MatrixStrideIterator& other) const { return _current < other._current; }
    inline bool operator>(const MatrixStrideIterator& other) const { return _current > other._current; }
    inline bool operator<=(const MatrixStrideIterator& other) const { return !(*this > other); }
    inline bool operator>=(const MatrixStrideIterator& other) const { return !(*this < other); }
};

#endif // MATRIX_STRIDE_ITERATOR_H
