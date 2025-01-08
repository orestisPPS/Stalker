#ifndef DENSE_TRIANGULAR_MATRIX_PTR_BUFFER_H
#define DENSE_TRIANGULAR_MATRIX_PTR_BUFFER_H

#include "../../Buffers/PtrBufferBase.h"
#include "../../Iterators/LinearStrideIterator.h"
#include "../../Iterators/FixedStrideIteratorBase.h"
#include "../MatrixTypes.h"

template <typename T, FormType FormT, OrderType OrderT, RegionType RegionT>
class DenseTriangularMatrixPtrBuffer : public PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormT, OrderT, RegionT>> {
    using Base = PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormT, OrderT, RegionT>>;

public:
    DenseTriangularMatrixPtrBuffer(T* data_start, std::size_t size) : Base(data_start, size, 1){}
    DenseTriangularMatrixPtrBuffer(const T* data_start, std::size_t size) : Base(const_cast<T*>(data_start), size, 1){}

protected:
    friend Base;
    inline T& _at(std::size_t i) { return *(this->_data_start + i * this->_stride); }
    inline const T& _at(std::size_t i) const { return *(this->_data_start + i * this->_stride); }
    inline FixedStrideIterator<T> _begin() { return FixedStrideIterator<T>(this->_data_start, this->_stride); }
    inline FixedStrideIterator<T> _end() { return FixedStrideIterator<T>(this->_data_start + this->_size * this->_stride, this->_stride); }
    inline FixedStrideIterator<const T> _cbegin() const { return FixedStrideIterator<const T>(this->_data_start, this->_stride); }
    inline FixedStrideIterator<const T> _cend() const { return FixedStrideIterator<const T>(this->_data_start + this->_size * this->_stride, this->_stride); }
};


template <typename T>
class DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::RowMajor, RegionType::Column>
    : public PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::RowMajor, RegionType::Column>> {
    using Base = PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::RowMajor, RegionType::Column>>;

public:
    DenseTriangularMatrixPtrBuffer(T* data_start, std::size_t size, std::size_t colIndex, std::size_t rows)
        : Base(data_start, size, 1), _rows(rows), _colIndex(colIndex) {}

    DenseTriangularMatrixPtrBuffer(const T* data_start, std::size_t size, std::size_t colIndex, std::size_t rows)
        : Base(const_cast<T*>(data_start), size, 1), _rows(rows), _colIndex(colIndex) {}

protected:
    friend Base;
    std::size_t _rows;
    std::size_t _colIndex;
    inline T& _at(std::size_t i) {
        return *(this->_data_start + i * (2 * _rows - i + 1) / 2 + (_colIndex - i));
    }
    inline const T& _at(std::size_t i) const {
        return *(this->_data_start + i * (2 * _rows - i + 1) / 2 + (_colIndex - i));
    }
    inline LinearStrideIterator<T, false> _begin() {
        return LinearStrideIterator<T, false>(this->_data_start + _colIndex, _rows - 1);
    }
    inline LinearStrideIterator<T, false> _end() {
        return LinearStrideIterator<T, false>(this->_data_start + _colIndex + 1, _rows - 1, _colIndex + 1);
    }
    inline LinearStrideIterator<const T, false> _cbegin() const {
        return LinearStrideIterator<const T, false>(this->_data_start + _colIndex, _rows - 1);
    }
    inline LinearStrideIterator<const T, false> _cend() const {
        return LinearStrideIterator<T, false>(this->_data_start + _colIndex + 1, _rows - 1, _colIndex + 1);
    }
};

template <typename T>
class DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>
    : public PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>> {
    using Base = PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>>;

public:
    DenseTriangularMatrixPtrBuffer(T* data_start, std::size_t size, std::size_t rows)
        : Base(data_start, size, 1), _rows(rows) {}

    DenseTriangularMatrixPtrBuffer(const T* data_start, std::size_t size, std::size_t rows)
        : Base(const_cast<T*>(data_start), size, 1), _rows(rows) {}

protected:
    friend Base;
    std::size_t _rows;
    inline T& _at(std::size_t i) { return *(this->_data_start + i); }
    inline const T& _at(std::size_t i) const { return *(this->_data_start + i); }
    inline LinearStrideIterator<T, true> _begin() { return LinearStrideIterator<T, true>(this->_data_start, 1); }
    inline LinearStrideIterator<T, true> _end() { return LinearStrideIterator<T, true>(this->_data_start + this->_size, 1); }
    inline LinearStrideIterator<const T, true> _cbegin() const { return LinearStrideIterator<const T, true>(this->_data_start, 1); }
    inline LinearStrideIterator<const T, true> _cend() const { return LinearStrideIterator<const T, true>(this->_data_start + this->_size, 1); }
};

template <typename T>
class DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::RowMajor, RegionType::Column>
    : public PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::RowMajor, RegionType::Column>> {
    using Base = PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::RowMajor, RegionType::Column>>;

public:
    DenseTriangularMatrixPtrBuffer(T* data_start, std::size_t size, std::size_t rows)
        : Base(data_start, size, 1), _rows(rows) {}

    DenseTriangularMatrixPtrBuffer(const T* data_start, std::size_t size, std::size_t rows)
        : Base(const_cast<T*>(data_start), size, 1), _rows(rows) {}

protected:
    friend Base;
    std::size_t _rows;

    inline T& _at(std::size_t i) { return *(this->_data_start + i); }
    inline const T& _at(std::size_t i) const { return *(this->_data_start + i); }
    inline LinearStrideIterator<T, false> _begin() { return LinearStrideIterator<T, false>(this->_data_start, _rows); }
    inline LinearStrideIterator<T, false> _end() { return LinearStrideIterator<T, false>(this->_data_start + this->_size, _rows); }
    inline LinearStrideIterator<const T, false> _cbegin() const { return LinearStrideIterator<const T, false>(this->_data_start, _rows); }
    inline LinearStrideIterator<const T, false> _cend() const { return LinearStrideIterator<const T, false>(this->_data_start + this->_size, _rows); }
};

template <typename T>
class DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::ColumnMajor, RegionType::Row>
    : public PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::ColumnMajor, RegionType::Row>> {
    using Base = PtrBufferBase<T, DenseTriangularMatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::ColumnMajor, RegionType::Row>>;

public:
    DenseTriangularMatrixPtrBuffer(T* data_start, std::size_t size, std::size_t rows)
        : Base(data_start, size, 1), _rows(rows) {}

    DenseTriangularMatrixPtrBuffer(const T* data_start, std::size_t size, std::size_t rows)
        : Base(const_cast<T*>(data_start), size, 1), _rows(rows) {}

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
