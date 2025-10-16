#ifndef DENSE_TRIANGULAR_MATRIX_PTR_BUFFER_H
#define DENSE_TRIANGULAR_MATRIX_PTR_BUFFER_H

#include "../../Buffers/PtrBufferBase.h"
#include "../../Iterators/LinearStrideIterator.h"
#include "../../Iterators/FixedStrideIteratorBase.h"
#include "../../Iterators/IndexDependentStrideIterator.h"
#include "../MatrixTypes.h"

template <typename T, StorageLayout Layout, T_Form FormT, OrderType OrderT, RegionType RegionT>
class MatrixPtrBuffer : public PtrBufferBase<T, MatrixPtrBuffer<T, Layout, FormT, OrderT, RegionT>> {
    using Base = PtrBufferBase<T, MatrixPtrBuffer<T, Layout, FormT, OrderT, RegionT>>;
    using Iterator = FixedStrideIterator<T>;
    using ConstIterator = FixedStrideIterator<const T>;

public:
    MatrixPtrBuffer(T* dataStart, std::size_t size) : Base(dataStart, size, 1){}
    MatrixPtrBuffer(const T* dataStart, std::size_t size) : Base(const_cast<T*>(dataStart), size, 1){}
    MatrixPtrBuffer(T* dataStart, std::size_t size, std::size_t primaryDimSize) : Base(dataStart, size, primaryDimSize){}
    MatrixPtrBuffer(const T* dataStart, std::size_t size, std::size_t primaryDimSize) : Base(const_cast<T*>(dataStart), size, primaryDimSize){}
    
    inline size_t startIndex() const { return 0; }
    inline size_t endIndex() const { return this->_size; }

protected:
    friend Base;
    inline T& _at(std::size_t i) {
        return *(this->_dataPtr + i * this->_stride);
    }
    inline const T& _at(std::size_t i) const {
        return *(this->_dataPtr + i * this->_stride);
    }
    inline Iterator _begin() {
        return Iterator(this->_dataPtr, this->_stride);
    }
    inline ConstIterator _cbegin() const {
        return ConstIterator(this->_dataPtr, this->_stride);
    }
    inline Iterator _end() {
        return Iterator(this->_dataPtr + this->_size * this->_stride, this->_stride);
    }
    inline ConstIterator _cend() const {
        return ConstIterator(this->_dataPtr + this->_size * this->_stride, this->_stride);
    }
};

template <typename T>
class MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor, RegionType::Column>
    : public PtrBufferBase<T, MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor, RegionType::Column>> {
    using Base = PtrBufferBase<T, MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor, RegionType::Column>>;
    using Iterator = LinearStrideIterator<T, false>;
    using ConstIterator = LinearStrideIterator<const T, false>;

public:
    MatrixPtrBuffer(T* matrixDataStart, std::size_t size, std::size_t colIndex, std::size_t rows)
        : Base(matrixDataStart, size, 1), _rows(rows), _colIndex(colIndex) {}

    MatrixPtrBuffer(const T* matrixDataStart, std::size_t size, std::size_t colIndex, std::size_t rows)
        : Base(const_cast<T*>(matrixDataStart), size, 1), _rows(rows), _colIndex(colIndex) {}
            
    inline size_t startIndex() const { return 0; }
    inline size_t endIndex() const { return _colIndex + 1; }

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
    inline Iterator _begin() {
        return Iterator(this->_dataPtr + _colIndex, _rows - 1);
    }
    inline ConstIterator _cbegin() const {
        return ConstIterator(this->_dataPtr + _colIndex, _rows - 1);
    }
    inline Iterator _end() {
        return Iterator(this->_dataPtr + _colIndex + 1, _rows - 1, _colIndex + 1);
    }
    inline ConstIterator _cend() const {
        return ConstIterator(this->_dataPtr + _colIndex + 1, _rows - 1, _colIndex + 1);
    }
};

template <typename T>
class MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>
    : public PtrBufferBase<T, MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>> {
    using Base = PtrBufferBase<T, MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>>;
    using Iterator = LinearStrideIterator<T, true>;
    using ConstIterator = LinearStrideIterator<const T, true>;

public:
    MatrixPtrBuffer(T* matrixDataStart, std::size_t size, std::size_t rowIndex, std::size_t rows)
        : Base(matrixDataStart, size, 1), _rows(rows), _rowIndex(rowIndex) {}

    MatrixPtrBuffer(const T* matrixDataStart, std::size_t size, std::size_t rowIndex, std::size_t rows)
        : Base(const_cast<T*>(matrixDataStart), size, 1), _rows(rows), _rowIndex(rowIndex) {}

    inline size_t startIndex() const { return _rowIndex; }
    inline size_t endIndex() const { return _rows - 1; }
    
protected:
    friend Base;
    std::size_t _rows;
    std::size_t _rowIndex;

    inline T& _at(std::size_t i) {
        i+=_rowIndex;
        return *(this->_dataPtr + i * (i + 1) / 2 + _rowIndex);
    }
    inline const T& _at(std::size_t i) const {
        i+=_rowIndex;
        return *(this->_dataPtr + i * (i + 1) / 2 + _rowIndex);
    }
    inline Iterator _begin() {
        return Iterator(this->_dataPtr + _rowIndex * (_rowIndex + 1) / 2 + _rowIndex, _rowIndex + 1, 0);
    }
    inline ConstIterator _cbegin() const {
        return ConstIterator(this->_dataPtr + _rowIndex * (_rowIndex + 1) / 2 + _rowIndex, _rowIndex + 1, 0);
    }
    inline Iterator _end() {
        return Iterator(this->_dataPtr + (this->_rows * (this->_rows + 1)) / 2, _rowIndex + 1, this->_rows - _rowIndex);
    }
    inline ConstIterator _cend() const {
        return ConstIterator(this->_dataPtr + (this->_rows * (this->_rows + 1)) / 2, _rowIndex + 1, this->_rows - _rowIndex);
    }
};

template <typename T>
class MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor, RegionType::Column>
  : public PtrBufferBase<T, MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor, RegionType::Column>>
{
    using Base = PtrBufferBase<T, MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor, RegionType::Column>>;
    using Iterator = IndexDependentStrideIterator<T, true>;
    using ConstIterator = IndexDependentStrideIterator<const T, true>;

public:
    MatrixPtrBuffer(T* matrixDataStart, std::size_t size, std::size_t colIndex, std::size_t rows)
        : Base(matrixDataStart, size, 1), _rows(rows), _colIndex(colIndex) {}

    MatrixPtrBuffer(const T* matrixDataStart, std::size_t size, std::size_t colIndex, std::size_t rows)
        : Base(const_cast<T*>(matrixDataStart), size, 1), _rows(rows), _colIndex(colIndex) {}

    inline size_t startIndex() const { return _colIndex; }
    inline size_t endIndex() const { return _rows - 1; }

protected:
    friend Base;

    std::size_t _rows;
    std::size_t _colIndex;

    inline T& _at(std::size_t i) {
        i+=_colIndex;
        return *(this->_dataPtr + i * (i + 1) / 2 + _colIndex);
    }
    inline const T& _at(std::size_t i) const {
        i+=_colIndex;
        return *(this->_dataPtr + i * (i + 1) / 2 + _colIndex);
    }

    inline Iterator _begin() {
        return Iterator(this->_dataPtr + _colIndex * (_colIndex + 1) / 2 + _colIndex, _colIndex + 1, 0);
    }

    inline ConstIterator _cbegin() const {
        return ConstIterator(this->_dataPtr + _colIndex * (_colIndex + 1) / 2 + _colIndex, _colIndex + 1, 0);
    }

    inline Iterator _end() {
        return Iterator(this->_dataPtr + (_rows - 1) * _rows / 2 + _colIndex + 1, _colIndex + 1, _rows - _colIndex);
    }

    inline ConstIterator _cend() const {
        return ConstIterator(this->_dataPtr + (_rows - 1) * _rows / 2 + _colIndex + 1, _colIndex + 1, _rows - _colIndex);
    }
};


template <typename T>
class MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor, RegionType::Row>
    : public PtrBufferBase<T, MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor, RegionType::Row>>
{
    using Base = PtrBufferBase<T, MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor, RegionType::Row>>;
    using Iterator = LinearStrideIterator<T, false>;
    using ConstIterator = LinearStrideIterator<const T, false>;

public:
    MatrixPtrBuffer(T* matrixDataStart, std::size_t size, std::size_t rowIndex, std::size_t rows)
        : Base(matrixDataStart, size, 1), _rows(rows), _rowIndex(rowIndex) {}

    MatrixPtrBuffer(const T* matrixDataStart, std::size_t size, std::size_t rowIndex, std::size_t rows)
        : Base(const_cast<T*>(matrixDataStart), size, 1), _rows(rows), _rowIndex(rowIndex) {}

    inline size_t startIndex() const { return 0; }
    inline size_t endIndex() const { return _rowIndex + 1; }
protected:
    friend Base;
    std::size_t _rows;
    std::size_t _rowIndex;

    inline T& _at(std::size_t j) {
        return *(this->_dataPtr + j * (2 * _rows - j + 1) / 2 + (_rowIndex - j));
    }

    inline const T& _at(std::size_t j) const {
        return *(this->_dataPtr + j * (2 * _rows - j + 1) / 2 + (_rowIndex - j));
    }

    inline Iterator _begin() {
        return Iterator(this->_dataPtr + _rowIndex, _rows - 1);
    }
    inline ConstIterator _cbegin() const {
        return ConstIterator(this->_dataPtr + _rowIndex, _rows - 1);
    }
    inline Iterator _end() {
        return Iterator(this->_dataPtr + _rowIndex + 1, _rows - 1, _rowIndex + 1);
    }
    inline ConstIterator _cend() const {
        return ConstIterator(this->_dataPtr + _rowIndex + 1, _rows - 1, _rowIndex + 1);
    }
};

#endif // DENSE_TRIANGULAR_MATRIX_PTR_BUFFER_H