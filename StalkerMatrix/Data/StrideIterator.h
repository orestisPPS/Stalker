#ifndef MATRIX_BLOCK_ITERATOR_BASE_H
#define MATRIX_BLOCK_ITERATOR_BASE_H

#include <iterator>
#include <cstddef> // For std::ptrdiff_t

/**
 * @brief Base class for CRTP-based matrix block iterators.
 * 
 * @tparam Derived The derived iterator class that inherits this base class.
 * @tparam T The type of elements being iterated over.
 * 
 * This base class provides core functionality for implementing iterators using the 
 * Curiously Recurring Template Pattern (CRTP). Derived classes define specific
 * traversal behaviors for contiguous or non-contiguous memory layouts.
 */
template <typename Derived, typename T>
class BlockIteratorBase {
protected:
    T* _current;       ///< Pointer to the current element.
    std::ptrdiff_t _stride; ///< Stride (step size) between consecutive elements.

private:
    /**
     * @brief Access the derived class via CRTP.
     * @return Reference to the derived class.
     */
    inline Derived& child() { return *static_cast<Derived*>(this); }

    /**
     * @brief Access the derived class (const version).
     * @return Const reference to the derived class.
     */
    inline const Derived& child() const { return *static_cast<const Derived*>(this); }

public:
    // STL-compliant iterator traits
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    /**
     * @brief Constructor to initialize the base iterator.
     * @param current Pointer to the initial element.
     * @param stride Stride (step size) between consecutive elements.
     */
    BlockIteratorBase(T* current, std::ptrdiff_t stride)
        : _current(current), _stride(stride) {}

    /**
     * @brief Dereference operator.
     * @return Reference to the element at the current position.
     */
    inline reference operator*() const { return *_current; }

    /**
     * @brief Member access operator.
     * @return Pointer to the element at the current position.
     */
    inline pointer operator->() const { return _current; }

    // Increment operators
    /**
     * @brief Pre-increment operator.
     * @return Reference to the updated iterator.
     */
    inline Derived& operator++() {
        child().increment();
        return child();
    }

    /**
     * @brief Post-increment operator.
     * @return A copy of the iterator before incrementing.
     */
    inline Derived operator++(int) {
        Derived temp = child();
        ++(*this);
        return temp;
    }

    // Decrement operators
    /**
     * @brief Pre-decrement operator.
     * @return Reference to the updated iterator.
     */
    inline Derived& operator--() {
        child().decrement();
        return child();
    }

    /**
     * @brief Post-decrement operator.
     * @return A copy of the iterator before decrementing.
     */
    inline Derived operator--(int) {
        Derived temp = child();
        --(*this);
        return temp;
    }

    // Addition and subtraction
    /**
     * @brief Advances the iterator by n elements.
     * @param n Number of elements to advance.
     * @return Updated iterator after advancing.
     */
    inline Derived operator+(difference_type n) const {
        Derived result = child();
        return result += n;
    }

    /**
     * @brief Advances the iterator by n elements in-place.
     * @param n Number of elements to advance.
     * @return Reference to the updated iterator.
     */
    inline Derived& operator+=(difference_type n) {
        child().advance(n);
        return child();
    }

    /**
     * @brief Moves the iterator backward by n elements.
     * @param n Number of elements to move back.
     * @return Updated iterator after decrementing.
     */
    inline Derived operator-(difference_type n) const {
        Derived result = child();
        return result -= n;
    }

    /**
     * @brief Moves the iterator backward by n elements in-place.
     * @param n Number of elements to move back.
     * @return Reference to the updated iterator.
     */
    inline Derived& operator-=(difference_type n) {
        return *this += -n;
    }

    // Distance and indexing
    /**
     * @brief Computes the distance between two iterators.
     * @param other The other iterator.
     * @return Difference in elements between this iterator and the other.
     */
    inline difference_type operator-(const Derived& other) const {
        return child().distance(other);
    }

    /**
     * @brief Access element at an offset relative to the current iterator.
     * @param n Offset from the current position.
     * @return Reference to the element at the offset position.
     */
    inline reference operator[](difference_type n) const {
        Derived temp = child();
        temp += n;
        return *temp;
    }

    // Comparison operators
    inline bool operator==(const Derived& other) const { return child().equals(other); }
    inline bool operator!=(const Derived& other) const { return !(*this == other); }
    inline bool operator<(const Derived& other) const { return child().less_than(other); }
    inline bool operator>(const Derived& other) const { return !(*this <= other); }
    inline bool operator<=(const Derived& other) const { return (*this < other) || (*this == other); }
    inline bool operator>=(const Derived& other) const { return !(*this < other); }
};

/**
 * @brief Iterator for traversing contiguous memory blocks.
 * 
 * @tparam T The type of elements being iterated over.
 * 
 * This iterator assumes the memory layout is contiguous with uniform spacing
 * (stride) between consecutive elements.
 */
template <typename T>
class ContiguousBlockIterator : public BlockIteratorBase<ContiguousBlockIterator<T>, T> {
    using Base = BlockIteratorBase<ContiguousBlockIterator<T>, T>;

public:
    /**
     * @brief Constructor to initialize the contiguous iterator.
     * @param current Pointer to the initial element.
     * @param stride Stride between consecutive elements.
     */
    ContiguousBlockIterator(T* current, std::ptrdiff_t stride)
        : Base(current, stride) {}

    // CRTP-required operations
    inline void increment() { this->_current += this->_stride; }
    inline void decrement() { this->_current -= this->_stride; }
    inline void advance(typename Base::difference_type n) { this->_current += n * this->_stride; }
    inline typename Base::difference_type distance(const ContiguousBlockIterator& other) const {
        return (this->_current - other._current) / this->_stride;
    }
    inline bool equals(const ContiguousBlockIterator& other) const { return this->_current == other._current; }
    inline bool less_than(const ContiguousBlockIterator& other) const { return this->_current < other._current; }
};

/**
 * @class NonContiguousBlockIterator
 * @brief An iterator for traversing non-contiguous blocks of data (e.g., columns in row-major order or rows in column-major order).
 * 
 * @tparam T The type of elements in the matrix.
 * 
 * This iterator facilitates traversal over non-contiguous blocks of memory, such as:
 * - Columns in a row-major ordered matrix.
 * - Rows in a column-major ordered matrix.
 * It maintains internal tracking of the current position in both primary and secondary dimensions.
 */
template <typename T>
class NonContiguousBlockIterator : public BlockIteratorBase<NonContiguousBlockIterator<T>, T> {
    using Base = BlockIteratorBase<NonContiguousBlockIterator<T>, T>;

private:
    T* _base;                ///< Pointer to the start of the data block.
    std::size_t _primaryDim; ///< Number of elements in the primary dimension.
    std::size_t _primaryIdx; ///< Current index in the primary dimension.
    std::size_t _secondaryIdx; ///< Current index in the secondary dimension.

public:
    /**
     * @brief Constructs a non-contiguous iterator for traversing a specific block of data.
     * 
     * @param base Pointer to the start of the memory block (typically the start of the matrix).
     * @param stride Stride between elements in the primary dimension.
     * @param primaryDim Total number of elements in the primary dimension.
     * @param primaryIdx Initial index in the primary dimension (e.g., starting row for a column).
     * @param secondaryIdx Initial index in the secondary dimension (e.g., column index for a column in row-major order).
     * 
     * ### Example:
     * #### Traversing a column in a row-major 4x3 matrix:
     * ```
     * Matrix:
     * 1  2  3
     * 4  5  6
     * 7  8  9
     * 10 11 12
     * 
     * NonContiguousBlockIterator<int> columnIter(&matrix[0], 3, 4, 0, 1);
     * ```
     * Here:
     * - `base` = Pointer to the first element of the matrix (`&matrix[0]`).
     * - `stride` = 3 (number of elements in a row).
     * - `primaryDim` = 4 (number of rows).
     * - `primaryIdx` = 0 (start at the first row).
     * - `secondaryIdx` = 1 (iterate over the second column).
     * 
     * #### Traversing a row in a column-major 3x4 matrix:
     * ```
     * Matrix:
     * 1  4  7  10
     * 2  5  8  11
     * 3  6  9  12
     * 
     * NonContiguousBlockIterator<int> rowIter(&matrix[0], 4, 3, 0, 1);
     * ```
     * Here:
     * - `base` = Pointer to the first element of the matrix (`&matrix[0]`).
     * - `stride` = 4 (number of rows).
     * - `primaryDim` = 3 (number of columns).
     * - `primaryIdx` = 0 (start at the first column).
     * - `secondaryIdx` = 1 (iterate over the second row).
     */
    NonContiguousBlockIterator(T* base, std::ptrdiff_t stride, std::size_t primaryDim, 
                               std::size_t primaryIdx, std::size_t secondaryIdx)
        : Base(base + primaryIdx * stride + secondaryIdx, stride),
          _base(base), _primaryDim(primaryDim), _primaryIdx(primaryIdx), _secondaryIdx(secondaryIdx) {}


    // CRTP-required operations
    inline void increment() {
        ++_primaryIdx;
        if (_primaryIdx >= _primaryDim) {
            _primaryIdx = 0;
            ++_secondaryIdx;
        }
        this->_current = _base + _primaryIdx * this->_stride + _secondaryIdx;
    }
    inline void decrement() {
        if (_primaryIdx == 0) {
            _primaryIdx = _primaryDim - 1;
            --_secondaryIdx;
        } else {
            --_primaryIdx;
        }
        this->_current = _base + _primaryIdx * this->_stride + _secondaryIdx;
    }
    inline void advance(typename Base::difference_type n) {
        typename Base::difference_type total = _primaryIdx + n;
        if (total >= 0) {
            _secondaryIdx += total / _primaryDim;
            _primaryIdx = total % _primaryDim;
        } else {
            _secondaryIdx += (total / _primaryDim) - 1;
            _primaryIdx = (total % _primaryDim + _primaryDim) % _primaryDim;
        }
        this->_current = _base + _primaryIdx * this->_stride + _secondaryIdx;
    }
    inline typename Base::difference_type distance(const NonContiguousBlockIterator& other) const {
        return (_secondaryIdx - other._secondaryIdx) * _primaryDim + (_primaryIdx - other._primaryIdx);
    }
    inline bool equals(const NonContiguousBlockIterator& other) const {
        return _primaryIdx == other._primaryIdx && _secondaryIdx == other._secondaryIdx;
    }
    inline bool less_than(const NonContiguousBlockIterator& other) const {
        return (_secondaryIdx < other._secondaryIdx) || 
               (_secondaryIdx == other._secondaryIdx && _primaryIdx < other._primaryIdx);
    }
};

#endif // MATRIX_BLOCK_ITERATOR_BASE_H
