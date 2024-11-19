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
     * @param startPtr Pointer to the initial element.
     * @param stride Stride (step size) between consecutive elements.
     */
    BlockIteratorBase(T* startPtr, std::ptrdiff_t stride)
        : _current(startPtr), _stride(stride) {}

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
class FixedStrideIterator : public BlockIteratorBase<FixedStrideIterator<T>, T> {
    using Base = BlockIteratorBase<FixedStrideIterator<T>, T>;

public:
    /**
     * @brief Constructor to initialize the contiguous iterator.
     * @param startPtr Pointer to the initial element.
     * @param stride Stride between consecutive elements.
     */
    FixedStrideIterator(T* startPtr, std::ptrdiff_t stride)
        : Base(startPtr, stride) {}

    // CRTP-required operations
    inline void increment() { this->_current += this->_stride; }
    inline void decrement() { this->_current -= this->_stride; }
    inline void advance(typename Base::difference_type n) { this->_current += n * this->_stride; }
    inline typename Base::difference_type distance(const FixedStrideIterator& other) const {
        return (this->_current - other._current) / this->_stride;
    }
    inline bool equals(const FixedStrideIterator& other) const { return this->_current == other._current; }
    inline bool less_than(const FixedStrideIterator& other) const { return this->_current < other._current; }
};


/**
 * @brief Iterator for traversing blocks with dynamic stride.
 *
 * @tparam Derived The derived iterator class.
 * @tparam T The type of elements being iterated over.
 *
 * This pure virtual iterator is the template for dynamic stride iterators, allowing variable steps between elements.
 * The stride is determined by the derived class, which can implement custom stride logic.
 * It is useful for non-linear traversal patterns in data structures like triangular matrices.
 */
template <typename Derived, typename T>
class DynamicStrideIterator : public BlockIteratorBase<Derived, T> {
    using Base = BlockIteratorBase<Derived, T>;

public:

    // CRTP-required operations
    inline void increment() {
        this->_current += child()._stride(_index);
        ++_index;
    }

    inline void decrement() {
        --_index;
        this->_current -= child()._stride(_index);
    }

    inline void advance(typename Base::difference_type n) {
        for (typename Base::difference_type i = 0; i < n; ++i) {
            this->_current += child()._stride();
            ++_index;
        }
    }

    inline typename Base::difference_type distance(const DynamicStrideIterator& other) const {
        return static_cast<typename Base::difference_type>(_index - other._index);
    }

    inline bool equals(const DynamicStrideIterator& other) const { return this->_current == other._current; }
    inline bool less_than(const DynamicStrideIterator& other) const { return this->_current < other._current; }

    size_t index() const { return _index; }

protected:
    DynamicStrideIterator(T* startPtr) : Base(startPtr), _index(0) {}

private:
    std::size_t _index; ///< Current logical index.
    inline Derived& child() { return *static_cast<Derived*>(this); }
    inline const Derived& child() const { return *static_cast<const Derived*>(this); }
};


#endif // MATRIX_BLOCK_ITERATOR_BASE_H
