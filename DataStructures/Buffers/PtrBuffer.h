#ifndef PTRBUFFER_H
#define PTRBUFFER_H

#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>
#include "../Iterators/StrideIterator.h"
#include "../../Utility/Checkers.h"
#include "../../GlobalDefinitions.h"

/**
 * @class PtrBufferBase
 * @brief Pure virtual base class for managing buffers with customizable stride.
 *
 * Provides core functionality for buffer management, including element access, size retrieval,
 * and iterator support. This class uses the CRTP (Curiously Recurring Template Pattern) to
 * allow derived classes to customize behavior.
 *
 * @tparam T Type of the elements in the buffer.
 * @tparam ChildPtrBuffer The derived buffer class implementing specific behavior.
 *
 * @note This class is intended to be used as a base for buffer types that require custom
 * iteration and access patterns for contiguous and non-contiguous memory blocks.
 */
template <typename T, typename ChildPtrBuffer>
class PtrBufferBase {
public:
    // STL-compliant container traits
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    /**
     * @brief Accesses the element at the specified index.
     *
     * @param i Index of the element.
     * @return Reference to the element.
     * @exception std::out_of_range If index is out of bounds.
     */
    inline T& operator[](std::size_t i) {
        if constexpr (RawDoggySize)
            checkIndex(i, _size, "PtrBufferBase::operator[]");
        return child()._at(i);
    }

    /**
     * @brief Accesses the element at the specified index (const version).
     *
     * @param i Index of the element.
     * @return Const reference to the element.
     * @exception std::out_of_range If index is out of bounds.
     */
    inline const T& operator[](std::size_t i) const {
        if constexpr (RawDoggyIndex)
            checkIndex(i, _size, "PtrBufferBase::operator[]");
        return child()._at(i);
    }

    /**
     * @brief Returns the number of elements in the buffer.
     * @return Size of the buffer.
     */
    inline size_t size() const { return _size; }

    /**
     * @brief Returns an iterator to the beginning of the buffer.
     * @return Iterator pointing to the first element.
     */
    inline auto begin() { return child()._begin(); }

    /**
     * @brief Returns an iterator to the end of the buffer.
     * @return Iterator pointing past the last element.
     */
    inline auto end() { return child()._end(); }

    /**
     * @brief Returns a const iterator to the beginning of the buffer.
     * @return Const iterator pointing to the first element.
     */
    inline auto cbegin() const { return child()._cbegin(); }

    /**
     * @brief Returns a const iterator to the end of the buffer.
     * @return Const iterator pointing past the last element.
     */
    inline auto cend() const { return child()._cend(); }

    inline std::vector<T> toVector() const {
        std::vector<T> vector(this->_size);
        std::memcpy(vector.data(), this->_data_start, this->_size * sizeof(T));
        return vector;
    }

protected:
    /**
     * @brief Constructs a buffer with the given parameters.
     *
     * @param data_start Pointer to the first element of the buffer.
     * @param size Number of elements in the buffer.
     * @param stride Stride between elements in memory.
     */
    PtrBufferBase(T* data_start, std::size_t size, std::ptrdiff_t stride)
        : _data_start(data_start), _size(size), _stride(stride) {}

    /**
     * @brief Constructs a const buffer with the given parameters.
     *
     * @param data_start Pointer to the first element of the const buffer.
     * @param size Number of elements in the buffer.
     * @param stride Stride between elements in memory.
     */
    PtrBufferBase(const T* data_start, std::size_t size, std::ptrdiff_t stride)
        : _data_start(const_cast<T*>(data_start)), _size(size), _stride(stride) {}

    T* _data_start;        ///< Pointer to the start of the buffer.
    std::size_t _size;     ///< Number of elements in the buffer.
    std::ptrdiff_t _stride; ///< Stride between elements.

    /**
     * @brief Access the derived class.
     * @return Reference to the derived buffer class.
     */
    inline ChildPtrBuffer& child() { return static_cast<ChildPtrBuffer&>(*this); }

    /**
     * @brief Access the derived class (const version).
     * @return Const reference to the derived buffer class.
     */
    inline const ChildPtrBuffer& child() const { return static_cast<const ChildPtrBuffer&>(*this); }
};


template <typename T>
class FixedStridePtrBuffer : public PtrBufferBase<T, FixedStridePtrBuffer<T>> {
public:

    FixedStridePtrBuffer(T* data_start, std::size_t size, std::ptrdiff_t stride = 1)
        : PtrBufferBase<T, FixedStridePtrBuffer<T>>(data_start, size, stride) {}

    FixedStridePtrBuffer(const T* data_start, std::size_t size, std::ptrdiff_t stride = 1)
        : PtrBufferBase<T, FixedStridePtrBuffer<T>>(const_cast<T*>(data_start), size, stride) {}


    inline T& _at(std::size_t i) {
        return *(this->_data_start + i * this->_stride);
    }
    inline const T& _at(std::size_t i) const {
        return *(this->_data_start + i * this->_stride);
    }
    inline FixedStrideIterator<T> _begin() {
        return FixedStrideIterator<T>(this->_data_start, this->_stride);
    }
    inline FixedStrideIterator<T> _end() {
        return FixedStrideIterator<T>(this->_data_start + this->_size * this->_stride, this->_stride);
    }
    inline FixedStrideIterator<const T> _cbegin() const {
        return FixedStrideIterator<const T>(this->_data_start, this->_stride);
    }
    inline FixedStrideIterator<const T> _cend() const {
        return FixedStrideIterator<const T>(this->_data_start + this->_size * this->_stride, this->_stride);
    }
};



#endif // PTRBUFFER_H