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

#ifndef MATRIXACCESSOR_TRIANGULARTSPECIALIZATION_H
#define MATRIXACCESSOR_TRIANGULARTSPECIALIZATION_H

#include "MatrixAccessor_FullTSpecialization.h"
#include "../Buffers/MatrixPtrBuffer.h"

template <typename T>
class MatrixAccessor<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor>
    : public MatrixAccessorBase<MatrixAccessor<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor>, T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor> {

    using Base = MatrixAccessorBase<MatrixAccessor<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor>, T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor>;
    using RowBuffer = MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor, RegionType::Row>;
    using ConstRowBuffer = MatrixPtrBuffer<const T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor, RegionType::Row>;
    using ColumnBuffer = MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor, RegionType::Column>;
    using ConstColumnBuffer = MatrixPtrBuffer<const T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor, RegionType::Column>;

public:

    MatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    MatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    MatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize) {
            checkSize(numValues, rows * (rows + 1) / 2, "MatrixAccessor:: Constructor");  
        }
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values; ///< Flat array for UpperTriangular matrix data

    inline T& _element(size_t i, size_t j) {
        if (i > j)
            return 0;
        return values[i * (2 * this->_rows - i + 1) / 2 + (j - i)];
    }
    inline const T& _element(size_t i, size_t j) const {
        if (i > j)
            return 0;
        return values[i * (2 * this->_rows - i + 1) / 2 + (j - i)];
    }
    inline RowBuffer _row(size_t row) {
        return RowBuffer(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row); 
    }
    inline ConstRowBuffer _row(size_t row) const {
        return ConstRowBuffer(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row);
    }
    inline ColumnBuffer _column(size_t col) {
        return ColumnBuffer(values.data(), col + 1, col, this->_rows);
    }
    inline ConstColumnBuffer _column(size_t col) const {
        return ConstColumnBuffer(values.data(), col + 1, col, this->_rows);
    }
};

template <typename T>
class MatrixAccessor<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor>
    : public MatrixAccessorBase<MatrixAccessor<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor>, T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor> {

    using Base = MatrixAccessorBase<MatrixAccessor<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor>, T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor>;
    using RowBuffer = MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>;
    using ConstRowBuffer = MatrixPtrBuffer<const T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor, RegionType::Row>;
    using ColumnBuffer = MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor, RegionType::Column>;
    using ConstColumnBuffer = MatrixPtrBuffer<const T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor, RegionType::Column>;

public:

    MatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    MatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    MatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize)
            checkSize(numValues, rows * (rows + 1) / 2, "MatrixAccessor:: Constructor");  
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values;

    inline T& _element(size_t i, size_t j) {
        if (i > j)
            return 0;
        return values[j * (j + 1) / 2 + i];
    }
    inline const T& _element(size_t i, size_t j) const {
        if (i > j)
            return 0;
        return values[j * (j + 1) / 2 + i];
    }
    inline RowBuffer _row(size_t row) {
        return RowBuffer(values.data(), this->_rows - row, row, this->_rows);
    }
    inline ConstRowBuffer _row(size_t row) const {
        return ConstRowBuffer(values.data(), this->_rows - row, row, this->_rows);
    }
    inline ColumnBuffer _column(size_t col) {
        return ColumnBuffer(values.data() + col * (col + 1) / 2, col + 1);
    }
    inline ConstColumnBuffer _column(size_t col) const {
        return ConstColumnBuffer(values.data() + col * (col + 1) / 2, col + 1);
    }
};

template <typename T>
class MatrixAccessor<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor>
    : public MatrixAccessorBase<MatrixAccessor<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor>, T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor> {

    using Base = MatrixAccessorBase<MatrixAccessor<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor>, T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor>;
    using RowBuffer = MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor, RegionType::Row>;
    using ConstRowBuffer = MatrixPtrBuffer<const T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor, RegionType::Row>;
    using ColumnBuffer = MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor, RegionType::Column>;
    using ConstColumnBuffer = MatrixPtrBuffer<const T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor, RegionType::Column>;

public:

    MatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    MatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    MatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize) {
            checkSize(numValues, rows * (rows + 1) / 2, "MatrixAccessor:: Constructor");  
        }
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values; ///< Flat array for LowerTriangular matrix data

    inline T& _element(size_t i, size_t j) {
        if (i < j)
            return 0;
        return values[i * (i + 1) / 2 + j];
    }
    inline const T& _element(size_t i, size_t j) const {
        if (i < j)
            return 0;
        return values[i * (i + 1) / 2 + j];
    }
    inline RowBuffer _row(size_t row) {
        return RowBuffer(values.data() + row * (row + 1) / 2, row + 1);
    }
    inline ConstRowBuffer _row(size_t row) const {
        return ConstRowBuffer(values.data() + row * (row + 1) / 2, row + 1);
    }
    inline ColumnBuffer _column(size_t col) {
        return ColumnBuffer(values.data(), this->_rows - col, col, this->_rows);
    }
    inline ConstColumnBuffer _column(size_t col) const {
        return ConstColumnBuffer(values.data(), this->_rows - col, col, this->_rows);
    }
};

template <typename T>
class MatrixAccessor<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor>
    : public MatrixAccessorBase<MatrixAccessor<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor>, T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor> {

    using Base = MatrixAccessorBase<MatrixAccessor<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor>, T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor>;
    using RowBuffer = MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor, RegionType::Row>;
    using ConstRowBuffer = MatrixPtrBuffer<const T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor, RegionType::Row>;
    using ColumnBuffer = MatrixPtrBuffer<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor, RegionType::Column>;
    using ConstColumnBuffer = MatrixPtrBuffer<const T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor, RegionType::Column>;

public:

    MatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    MatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    MatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize)
            checkSize(numValues, rows * (rows + 1) / 2, "MatrixAccessor:: Constructor");  
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values;

    inline T& _element(size_t i, size_t j) {
        if (i < j)
            return 0;
        return values[j * (2 * this->_rows - j + 1) / 2 + (i - j)];
    }
    inline const T& _element(size_t i, size_t j) const {
        if (i < j)
            return 0;
        return values[j * (2 * this->_rows - j + 1) / 2 + (i - j)];
    }

    inline RowBuffer _row(size_t row) {
        return RowBuffer(values.data(), row + 1, row, this->_rows);
    }
    inline ConstRowBuffer _row(size_t row) const {
        return ConstRowBuffer(values.data(), row + 1, row, this->_rows);
    }
    inline ColumnBuffer _column(size_t col) {
        return ColumnBuffer(values.data() + col * (2 * this->_rows - col + 1) / 2, this->_rows - col);
    }
    inline ConstColumnBuffer _column(size_t col) const {
        return ConstColumnBuffer(values.data() + col * (2 * this->_rows - col + 1) / 2, this->_rows - col);
    }
};



#endif // MATRIXACCESSOR_TRIANGULARTSPECIALIZATION_H