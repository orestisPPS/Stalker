/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <iterator>
#include <cstddef> // For std::ptrdiff_t

namespace Stalker::Containers::Iterators {

/**
 * @brief Base class for CRTP-based memory iterators.
 * 
 * @tparam Derived The derived iterator class that inherits this base class.
 * @tparam T The type of elements being iterated over.
 * 
 * This base class provides core functionality for implementing iterators using the 
 * Curiously Recurring Template Pattern (CRTP). Derived classes define specific
 * traversal behaviors for contiguous or non-contiguous memory layouts.
 */
template <typename Derived, typename T>
class StrideIteratorBase {
    
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
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    StrideIteratorBase(U* startPtr, std::ptrdiff_t stride)
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

    /**
     * @brief Pre-increment operator.
     * @return Reference to the updated iterator.
     */
    inline Derived& operator++() {
        child()._increment();
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

    /**
     * @brief Pre-decrement operator.
     * @return Reference to the updated iterator.
     */
    inline Derived& operator--() {
        child()._decrement();
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
        child()._advance(n);
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
        return child()._distance(other);
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
    inline bool operator==(const Derived& other) const { return child()._equals(other); }
    inline bool operator!=(const Derived& other) const { return !(*this == other); }
    inline bool operator<(const Derived& other) const { return child()._less_than(other); }
    inline bool operator>(const Derived& other) const { return !(*this <= other); }
    inline bool operator<=(const Derived& other) const { return (*this < other) || (*this == other); }
    inline bool operator>=(const Derived& other) const { return !(*this < other); }
};



} // namespace Stalker::Containers::Iterators
