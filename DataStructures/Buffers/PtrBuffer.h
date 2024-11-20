#ifndef MATRIX_PTRBUFFER_H
#define MATRIX_PTRBUFFER_H

#include <cstddef>
#include <stdexcept>
#include <vector>
#include <cstring>
#include "../Iterators/StrideIterator.h"

/**
 * @class PtrBufferBase
 * @brief Base class for managing buffers with customizable stride.
 *
 * Provides core functionality for buffer management, including element access, size retrieval,
 * and iterator support. This class uses the CRTP (Curiously Recurring Template Pattern) to
 * allow derived classes to customize behavior.
 *
 * @tparam T Type of the elements in the buffer.
 * @tparam ChildPtrBuffer The derived buffer class implementing specific behavior.
 *
 * @note This class is intended to be used as a base for buffer types that require custom
 * iteration and access patterns, such as contiguous and non-contiguous blocks in matrices.
 */
template <typename T, typename ChildPtrBuffer>
class PtrBufferBase {
public:
    /**
     * @brief Constructs a buffer with the given parameters.
     *
     * @param data_start Pointer to the first element of the buffer.
     * @param size Number of elements in the buffer.
     * @param stride Stride between elements in memory (default is 1).
     */
    PtrBufferBase(T* data_start, std::size_t size, std::ptrdiff_t stride = 1)
        : _data_start(data_start), _size(size), _stride(stride) {}

    /**
     * @brief Constructs a const buffer with the given parameters.
     *
     * @param data_start Pointer to the first element of the const buffer.
     * @param size Number of elements in the buffer.
     * @param stride Stride between elements in memory (default is 1).
     */
    PtrBufferBase(const T* data_start, std::size_t size, std::ptrdiff_t stride = 1)
        : _data_start(const_cast<T*>(data_start)), _size(size), _stride(stride) {}

    /**
     * @brief Accesses the element at the specified index.
     *
     * @param i Index of the element.
     * @return Reference to the element.
     * @exception std::out_of_range If index is out of bounds.
     */
    inline T& operator[](std::size_t i) {
        if (i >= _size)
            throw std::out_of_range("PtrBufferBase index out of range");
        return child()[i];
    }

    /**
     * @brief Accesses the element at the specified index (const version).
     *
     * @param i Index of the element.
     * @return Const reference to the element.
     * @exception std::out_of_range If index is out of bounds.
     */
    inline const T& operator[](std::size_t i) const {
        if (i >= _size)
            throw std::out_of_range("PtrBufferBase index out of range");
        return child()[i];
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
    inline auto begin() { return child().begin(); }

    /**
     * @brief Returns an iterator to the end of the buffer.
     * @return Iterator pointing past the last element.
     */
    inline auto end() { return child().end(); }

    /**
     * @brief Returns a const iterator to the beginning of the buffer.
     * @return Const iterator pointing to the first element.
     */
    inline auto cbegin() const { return child().cbegin(); }

    /**
     * @brief Returns a const iterator to the end of the buffer.
     * @return Const iterator pointing past the last element.
     */
    inline auto cend() const { return child().cend(); }

    inline std::vector<T> toVector() const {
        std::vector<T> vector(this->_size);
        std::memcpy(vector.data(), this->_data_start, this->_size * sizeof(T));
        return vector;
    }

protected:
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


/**
 * @class FixedStridePtrBuffer
 * @brief Represents a contiguous block of matrix data.
 * 
 * This buffer is used to access contiguous blocks of data such as rows in row-major matrices
 * or columns in column-major matrices. The memory layout of the block must be sequential.
 * 
 * @tparam T Type of the elements in the buffer.
 */
template <typename T>
class FixedStridePtrBuffer : public PtrBufferBase<T, FixedStridePtrBuffer<T>> {
public:
    /**
     * @brief Constructs a buffer for a contiguous block.
     * 
     * Use this constructor when the block's memory is contiguous (e.g., rows in a row-major matrix).
     * 
     * @param data_start Pointer to the first element of the block.
     * @param size Number of elements in the block.
     * @param stride Distance (in elements) between consecutive elements in the block (default is 1).
     * 
     * @example Accessing a read-only row:
     * ```cpp
     * double matrix[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
     * FixedStridePtrBuffer<const double> rowBuffer(matrix + 3, 3);
     * // Buffer contains: 4, 5, 6
     * ```
     */
    FixedStridePtrBuffer(T* data_start, std::size_t size, std::ptrdiff_t stride = 1)
        : PtrBufferBase<T, FixedStridePtrBuffer<T>>(data_start, size, stride) {}

    /**
     * @brief Constructs a const buffer for a contiguous block.
     * 
     * This overload is used for blocks that should remain immutable (e.g., read-only rows).
     * 
     * @param data_start Pointer to the first element of the const block.
     * @param size Number of elements in the block.
     * @param stride Distance (in elements) between consecutive elements in the block (default is 1).
     * 
     * @example Accessing a read-only row:
     * ```cpp
     * double matrix[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
     * FixedStridePtrBuffer<const double> rowBuffer(matrix + 3, 3);
     * // Buffer contains: 4, 5, 6
     * ```
     */
    FixedStridePtrBuffer(const T* data_start, std::size_t size, std::ptrdiff_t stride = 1)
        : PtrBufferBase<T, FixedStridePtrBuffer<T>>(const_cast<T*>(data_start), size, stride) {}


    inline T& operator[](std::size_t i) { 
        return *(this->_data_start + i * this->_stride); 
    }

    inline const T& operator[](std::size_t i) const { 
        return *(this->_data_start + i * this->_stride); 
    }

    inline T& at(std::size_t i) {
        if (i >= this->_size)
        throw std::out_of_range("MatrixPtrBuffer index out of range");
        return *(this->_data_start + i * this->_stride); 
    }

    inline const T& at(std::size_t i) const {
        if (i >= this->_size)
        throw std::out_of_range("MatrixPtrBuffer index out of range");
        return *(this->_data_start + i * this->_stride); 
    }

    inline FixedStrideIterator<T> begin() {
        return FixedStrideIterator<T>(this->_data_start, this->_stride);
    }

    inline FixedStrideIterator<T> end() {
        return FixedStrideIterator<T>(this->_data_start + this->_size * this->_stride, this->_stride);
    }

    inline FixedStrideIterator<const T> cbegin() const {
        return FixedStrideIterator<const T>(this->_data_start, this->_stride);
    }

    inline FixedStrideIterator<const T> cend() const {
        return FixedStrideIterator<const T>(this->_data_start + this->_size * this->_stride, this->_stride);
    }
};

#endif // MATRIX_PTRBUFFER_H