#ifndef STALKER_MATRIX_BUFFERS_H
#define STALKER_MATRIX_BUFFERS_H

#include "../../Buffers/PtrBuffer.h"
#include "../MatrixTypes.h"

template <typename T>
class GeneralMatrixPtrBuffer : public PtrBufferBase<T, GeneralMatrixPtrBuffer<T>> {
    using Base = PtrBufferBase<T, GeneralMatrixPtrBuffer<T>>;

public:

    /**
     * @brief A buffer class for full dense matrix pointers.
     * 
     * This class provides a buffer for full dense matrix pointers, inheriting from GeneralMatrixPtrBufferBase.
     * It allows for the creation of buffers for rows or columns from both row-major and column-major full matrices.
     * 
     * The class supports the following constructors:
     * - Constructors  that initializes the buffer with a given data start pointer / constant pointer, size, and stride.
     * - Constructors that initializes the buffer with a given data start pointer / constant pointer, size, primary dimension size, and stride.
     * 
     * @tparam T The type of the elements in the matrix.
     */
    GeneralMatrixPtrBuffer(T* data_start, std::size_t size, std::ptrdiff_t _stride = 1) : Base(data_start, size, _stride) {}
        
    /**
     * @brief Constructs a GeneralMatrixPtrBuffer with a given constant data start pointer, size, and stride.
     *
     * This constructor initializes a GeneralMatrixPtrBuffer to buffer a row from a row-major full matrix
     * or a column from a column-major full matrix.
     * 
     * @param data_start Constant pointer to the start of the data.
     * @param size The size of the matrix.
     * @param stride The stride between elements (default is 1).
     */
    GeneralMatrixPtrBuffer(const T* data_start, std::size_t size, std::ptrdiff_t _stride) : Base(const_cast<T*>(data_start), size, _stride) {}

    /**
     * @brief Constructs a GeneralMatrixPtrBuffer with a given data start pointer, size, primary dimension size, and stride.
     *
     * This constructor initializes a GeneralMatrixPtrBuffer to buffer a row from a column-major full matrix
     * or a column from a row-major full matrix.
     * 
     * @param data_start Pointer to the start of the data.
     * @param size The size of the matrix.
     * @param primaryDimSize The size of the primary dimension. Number of rows for row-major matrices, number of columns for column-major matrices.
     * @param stride The stride between elements (default is 1).
     */
    GeneralMatrixPtrBuffer(T* data_start, std::size_t size, std::size_t primaryDimSize, std::ptrdiff_t stride = 1) : Base(data_start, size, primaryDimSize + stride) {}

    /**
     * @brief Constructs a GeneralMatrixPtrBuffer with a given constant data start pointer, size, primary dimension size, and stride.
     *
     * This constructor initializes a GeneralMatrixPtrBuffer to buffer a row from a column-major full matrix 
     * or a column from a row-major full matrix.
     * 
     * @param data_start Constant pointer to the start of the data.
     * @param size The size of the matrix.
     * @param primaryDimSize The size of the primary dimension. Number of rows for row-major matrices, number of columns for column-major matrices.
     * @param stride The stride between elements (default is 1).
     */
    GeneralMatrixPtrBuffer(const T* data_start, std::size_t size, std::size_t primaryDimSize, std::ptrdiff_t stride = 1) : Base(const_cast<T*>(data_start), size, primaryDimSize + stride) {}

    inline T& _at(std::size_t i) { return *(this->_data_start + i * this->_stride); }
    inline const T& _at(std::size_t i) const { return *(this->_data_start + i * this->_stride); }
    inline FixedStrideIterator<T> _begin() { return FixedStrideIterator<T>(this->_data_start, this->_stride); }
    inline FixedStrideIterator<T> _end() { return FixedStrideIterator<T>(this->_data_start + this->_size * this->_stride, this->_stride); }
    inline FixedStrideIterator<const T> _cbegin() const { return FixedStrideIterator<const T>(this->_data_start, this->_stride); }
    inline FixedStrideIterator<const T> _cend() const { return FixedStrideIterator<const T>(this->_data_start + this->_size * this->_stride, this->_stride); }

};

#endif // STALKER_MATRIX_BUFFERS_H