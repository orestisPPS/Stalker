#ifndef DENSE_TRIANGULAR_MATRIX_PTR_BUFFER_H
#define DENSE_TRIANGULAR_MATRIX_PTR_BUFFER_H

#include "../../Buffers/PtrBufferBase.h"
#include "../../Iterators/LinearStrideIterator.h"
#include "../../Iterators/FixedStrideIteratorBase.h"
#include "../MatrixTypes.h"

template <typename T, FormType FormT, OrderType OrderT, RegionType RegionT>
class MatrixPtrBuffer : public PtrBufferBase<T, MatrixPtrBuffer<T, FormT, OrderT, RegionT>> {
    using Base = PtrBufferBase<T, MatrixPtrBuffer<T, FormT, OrderT, RegionT>>;

public:
    MatrixPtrBuffer(T* dataStart, std::size_t size) : Base(dataStart, size, 1){}
    MatrixPtrBuffer(const T* dataStart, std::size_t size) : Base(const_cast<T*>(dataStart), size, 1){}
    MatrixPtrBuffer(T* dataStart, std::size_t size, std::size_t primaryDimSize) : Base(dataStart, size, primaryDimSize){}
    MatrixPtrBuffer(const T* dataStart, std::size_t size, std::size_t primaryDimSize) : Base(const_cast<T*>(dataStart), size, primaryDimSize){}

protected:
    friend Base;
    inline T& _at(std::size_t i) { return *(this->_dataPtr + i * this->_stride); }
    inline const T& _at(std::size_t i) const { return *(this->_dataPtr + i * this->_stride); }
    inline FixedStrideIterator<T> _begin() { return FixedStrideIterator<T>(this->_dataPtr, this->_stride); }
    inline FixedStrideIterator<T> _end() { return FixedStrideIterator<T>(this->_dataPtr + this->_size * this->_stride, this->_stride); }
    inline FixedStrideIterator<const T> _cbegin() const { return FixedStrideIterator<const T>(this->_dataPtr, this->_stride); }
    inline FixedStrideIterator<const T> _cend() const { return FixedStrideIterator<const T>(this->_dataPtr + this->_size * this->_stride, this->_stride); }
};


template <typename T>
class MatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::RowMajor, RegionType::Column>
    : public PtrBufferBase<T, MatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::RowMajor, RegionType::Column>> {
    using Base = PtrBufferBase<T, MatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::RowMajor, RegionType::Column>>;

public:
    MatrixPtrBuffer(T* matrixDataStart, std::size_t size, std::size_t colIndex, std::size_t rows)
        : Base(matrixDataStart, size, 1), _rows(rows), _colIndex(colIndex) {}

    MatrixPtrBuffer(const T* matrixDataStart, std::size_t size, std::size_t colIndex, std::size_t rows)
        : Base(const_cast<T*>(matrixDataStart), size, 1), _rows(rows), _colIndex(colIndex) {}

protected:
    friend Base;
    std::size_t _rows;
    std::size_t _colIndex;
    inline T& _at(std::size_t i) {
        return *(this->_dataPtr + i * (2 * _rows - i + 1) / 2 + (_colIndex - i));
    }
    inline const T& _at(std::size_t i) const {
        return *(this->_dataPtr + i * (2 * _rows - i + 1) / 2 + (_colIndex - i));
    }
    inline LinearStrideIterator<T, false> _begin() {
        return LinearStrideIterator<T, false>(this->_dataPtr + _colIndex, _rows - 1);
    }
    inline LinearStrideIterator<T, false> _end() {
        return LinearStrideIterator<T, false>(this->_dataPtr + _colIndex + 1, _rows - 1, _colIndex + 1);
    }
    inline LinearStrideIterator<const T, false> _cbegin() const {
        return LinearStrideIterator<const T, false>(this->_dataPtr + _colIndex, _rows - 1);
    }
    inline LinearStrideIterator<const T, false> _cend() const {
        return LinearStrideIterator<T, false>(this->_dataPtr + _colIndex + 1, _rows - 1, _colIndex + 1);
    }
};

template <typename T>
class MatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>
    : public PtrBufferBase<T, MatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>> {
    using Base = PtrBufferBase<T, MatrixPtrBuffer<T, FormType::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>>;

public:
    MatrixPtrBuffer(T* matrixDataStart, std::size_t size, std::size_t rowIndex, std::size_t rows)
        : Base(matrixDataStart, size, 1), _rows(rows), _rowIndex(rowIndex) {}

    MatrixPtrBuffer(const T* matrixDataStart, std::size_t size, std::size_t rowIndex, std::size_t rows)
        : Base(const_cast<T*>(matrixDataStart), size, 1), _rows(rows), _rowIndex(rowIndex) {}

protected:
    friend Base;
    std::size_t _rows;
    std::size_t _rowIndex;
    inline T& _at(std::size_t i) { return *(this->_dataPtr + i * (i + 1) / 2 + _rowIndex); }
    inline const T& _at(std::size_t i) const { return *(this->_dataPtr + i * (i + 1) / 2 + _rowIndex); }
    inline LinearStrideIterator<T, true> _begin() { return LinearStrideIterator<T, true>(this->_dataPtr + _rowIndex * (_rowIndex + 1) / 2 + _rowIndex, 1); }
    inline LinearStrideIterator<T, true> _end() { return LinearStrideIterator<T, true>(this->_dataPtr + (this->_rows - 1) * this->_rows / 2 + _rowIndex, 1); }
    inline LinearStrideIterator<const T, true> _cbegin() const { return LinearStrideIterator<const T, true>(this->_dataPtr + _rowIndex * (_rowIndex + 1) / 2 + _rowIndex, _rowIndex); }
    inline LinearStrideIterator<const T, true> _cend() const { return LinearStrideIterator<const T, true>(this->_dataPtr + (this->_rows - 1) * this->_rows / 2 + _rowIndex, _rowIndex); }
};

template <typename T>
class MatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::RowMajor, RegionType::Column>
    : public PtrBufferBase<T, MatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::RowMajor, RegionType::Column>> {
    using Base = PtrBufferBase<T, MatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::RowMajor, RegionType::Column>>;

public:
    MatrixPtrBuffer(T* matrixDataStart, std::size_t size, std::size_t rows)
        : Base(matrixDataStart, size, 1), _rows(rows) {}

    MatrixPtrBuffer(const T* matrixDataStart, std::size_t size, std::size_t rows)
        : Base(const_cast<T*>(matrixDataStart), size, 1), _rows(rows) {}

protected:
    friend Base;
    std::size_t _rows;

    inline T& _at(std::size_t i) { return *(this->_dataPtr + i); }
    inline const T& _at(std::size_t i) const { return *(this->_dataPtr + i); }
    inline LinearStrideIterator<T, false> _begin() { return LinearStrideIterator<T, false>(this->_dataPtr, _rows); }
    inline LinearStrideIterator<T, false> _end() { return LinearStrideIterator<T, false>(this->_dataPtr + this->_size, _rows); }
    inline LinearStrideIterator<const T, false> _cbegin() const { return LinearStrideIterator<const T, false>(this->_dataPtr, _rows); }
    inline LinearStrideIterator<const T, false> _cend() const { return LinearStrideIterator<const T, false>(this->_dataPtr + this->_size, _rows); }
};

template <typename T>
class MatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::ColumnMajor, RegionType::Row>
    : public PtrBufferBase<T, MatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::ColumnMajor, RegionType::Row>> {
    using Base = PtrBufferBase<T, MatrixPtrBuffer<T, FormType::LowerTriangular, OrderType::ColumnMajor, RegionType::Row>>;

public:
    MatrixPtrBuffer(T* matrixDataStart, std::size_t size, std::size_t rows)
        : Base(matrixDataStart, size, 1), _rows(rows) {}

    MatrixPtrBuffer(const T* matrixDataStart, std::size_t size, std::size_t rows)
        : Base(const_cast<T*>(matrixDataStart), size, 1), _rows(rows) {}

protected:
    friend Base;
    std::size_t _rows;

    inline T& _at(std::size_t i) { return *(this->_dataPtr + i); }
    inline const T& _at(std::size_t i) const { return *(this->_dataPtr + i); }
    inline LinearStrideIterator<T, true> _begin() { return LinearStrideIterator<T, true>(this->_dataPtr, 1, -1); }
    inline LinearStrideIterator<T, true> _end() { return LinearStrideIterator<T, true>(this->_dataPtr + this->_size, 1, -1); }
    inline LinearStrideIterator<const T, true> _cbegin() const { return LinearStrideIterator<const T, true>(this->_dataPtr, 1, -1); }
    inline LinearStrideIterator<const T, true> _cend() const { return LinearStrideIterator<const T, true>(this->_dataPtr + this->_size, 1, -1); }
};

#endif // DENSE_TRIANGULAR_MATRIX_PTR_BUFFER_H