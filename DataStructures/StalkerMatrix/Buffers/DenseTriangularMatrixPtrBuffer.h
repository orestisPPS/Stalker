#ifndef DENSE_TRIANGULAR_MATRIX_PTR_BUFFER_H
#define DENSE_TRIANGULAR_MATRIX_PTR_BUFFER_H

#include "../../Buffers/PtrBufferBase.h"
#include "../../Iterators/LinearStrideIterator.h"
#include "../MatrixTypes.h"

template <typename T, FormType FormT, OrderType OrderT>
class DenseTriangularMatrixPtrBuffer : public PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormT, OrderT>> {
    using Base = PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormT, OrderT>>;

public:
    DenseTriangularMatrixPtrBuffer(T* data_start, std::size_t size, std::size_t rows, std::ptrdiff_t stride = 1)
        : Base(data_start, size, stride), _rows(rows) {}

    DenseTriangularMatrixPtrBuffer(const T* data_start, std::size_t size, std::size_t rows, std::ptrdiff_t stride = 1)
        : Base(const_cast<T*>(data_start), size, stride), _rows(rows) {}

protected:
    friend Base;
    std::size_t _rows; ///< Total number of rows in the triangular matrix.

    inline T& _at(std::size_t i) { return *(this->_data_start + i); }
    inline const T& _at(std::size_t i) const { return *(this->_data_start + i); }
    inline LinearStrideIterator<T, true> _begin() { return LinearStrideIterator<T, true>(this->_data_start, _rows, -1); }
    inline LinearStrideIterator<T, true> _end() { return LinearStrideIterator<T, true>(this->_data_start + this->_size, _rows, -1); }
    inline LinearStrideIterator<const T, true> _cbegin() const { return LinearStrideIterator<const T, true>(this->_data_start, _rows, -1); }
    inline LinearStrideIterator<const T, true> _cend() const { return LinearStrideIterator<const T, true>(this->_data_start + this->_size, _rows, -1); }
};
template <typename T>
class DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor>
    : public PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor>> {
    using Base = PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor>>;

public:
    DenseTriangularMatrixPtrBuffer(T* data_start, std::size_t size, std::size_t rows, std::ptrdiff_t stride = 1)
        : Base(data_start, size, stride), _rows(rows) {}

    DenseTriangularMatrixPtrBuffer(const T* data_start, std::size_t size, std::size_t rows, std::ptrdiff_t stride = 1)
        : Base(const_cast<T*>(data_start), size, stride), _rows(rows) {}

protected:
    friend Base;
    std::size_t _rows;
    inline T& _at(std::size_t i) { return *(this->_data_start + i); }
    inline const T& _at(std::size_t i) const { return *(this->_data_start + i); }
    inline LinearStrideIterator<T, true> _begin() { return LinearStrideIterator<T, true>(this->_data_start, 1, 1); }
    inline LinearStrideIterator<T, true> _end() { return LinearStrideIterator<T, true>(this->_data_start + this->_size, 1, 1); }
    inline LinearStrideIterator<const T, true> _cbegin() const { return LinearStrideIterator<const T, true>(this->_data_start, 1, 1); }
    inline LinearStrideIterator<const T, true> _cend() const { return LinearStrideIterator<const T, true>(this->_data_start + this->_size, 1, 1); }
};

template <typename T>
class DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::RowMajor>
    : public PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::RowMajor>> {
    using Base = PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::RowMajor>>;

public:
    DenseTriangularMatrixPtrBuffer(T* data_start, std::size_t size, std::size_t rows, std::ptrdiff_t stride = 1)
        : Base(data_start, size, stride), _rows(rows) {}

    DenseTriangularMatrixPtrBuffer(const T* data_start, std::size_t size, std::size_t rows, std::ptrdiff_t stride = 1)
        : Base(const_cast<T*>(data_start), size, stride), _rows(rows) {}

protected:
    friend Base;
    std::size_t _rows;

    inline T& _at(std::size_t i) { return *(this->_data_start + i); }
    inline const T& _at(std::size_t i) const { return *(this->_data_start + i); }
    inline LinearStrideIterator<T, false> _begin() { return LinearStrideIterator<T, false>(this->_data_start, _rows, 1); }
    inline LinearStrideIterator<T, false> _end() { return LinearStrideIterator<T, false>(this->_data_start + this->_size, _rows, 1); }
    inline LinearStrideIterator<const T, false> _cbegin() const { return LinearStrideIterator<const T, false>(this->_data_start, _rows, 1); }
    inline LinearStrideIterator<const T, false> _cend() const { return LinearStrideIterator<const T, false>(this->_data_start + this->_size, _rows, 1); }
};

template <typename T>
class DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::ColumnMajor>
    : public PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::ColumnMajor>> {
    using Base = PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::ColumnMajor>>;

public:
    DenseTriangularMatrixPtrBuffer(T* data_start, std::size_t size, std::size_t rows, std::ptrdiff_t stride = 1)
        : Base(data_start, size, stride), _rows(rows) {}

    DenseTriangularMatrixPtrBuffer(const T* data_start, std::size_t size, std::size_t rows, std::ptrdiff_t stride = 1)
        : Base(const_cast<T*>(data_start), size, stride), _rows(rows) {}

protected:
    friend Base;
    std::size_t _rows;

    inline T& _at(std::size_t i) { return *(this->_data_start + i); }
    inline const T& _at(std::size_t i) const { return *(this->_data_start + i); }
    inline LinearStrideIterator<T, true> _begin() { return LinearStrideIterator<T, true>(this->_data_start, 1, -1); }
    inline LinearStrideIterator<T, true> _end() { return LinearStrideIterator<T, true>(this->_data_start + this->_size, 1, -1); }
    inline LinearStrideIterator<const T, true> _cbegin() const { return LinearStrideIterator<const T, true>(this->_data_start, 1, -1); }
    inline LinearStrideIterator<const T, true> _cend() const { return LinearStrideIterator<const T, true>(this->_data_start + this->_size, 1, -1); }
};

#endif // DENSE_TRIANGULAR_MATRIX_PTR_BUFFER_H
