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

#ifndef CompressedSparseMATRIXACCESSOR_COMPRESSEDSPARSETSPECIALIZATION_H
#define CompressedSparseMATRIXACCESSOR_COMPRESSEDSPARSETSPECIALIZATION_H

#include "MatrixAccessor_FullTSpecialization.h"

template <typename T, T_Form FormT>
class MatrixAccessor<T, StorageLayout::CompressedSparse, FormT, OrderType::RowMajor>
    : public MatrixAccessorBase<MatrixAccessor<T, StorageLayout::CompressedSparse, FormT, OrderType::RowMajor>, T, StorageLayout::CompressedSparse, FormT, OrderType::RowMajor> {

    using Base = MatrixAccessorBase<MatrixAccessor<T, StorageLayout::CompressedSparse, FormT, OrderType::RowMajor>, T, StorageLayout::CompressedSparse, FormT, OrderType::RowMajor>;
    using RowBuffer = MatrixPtrBuffer<T, StorageLayout::CompressedSparse, T_Form::UpperTriangular, OrderType::RowMajor, RegionType::Row>;
    using ConstRowBuffer = MatrixPtrBuffer<const T, StorageLayout::CompressedSparse, T_Form::UpperTriangular, OrderType::RowMajor, RegionType::Row>;
    using ColumnBuffer = MatrixPtrBuffer<T, StorageLayout::CompressedSparse, T_Form::UpperTriangular, OrderType::RowMajor, RegionType::Column>;
    using ConstColumnBuffer = MatrixPtrBuffer<const T, StorageLayout::CompressedSparse, T_Form::UpperTriangular, OrderType::RowMajor, RegionType::Column>;

public:

    MatrixAccessor(size_t rows, size_t cols) : Base(rows, cols) {}
    
    MatrixAccessor(size_t   rows,
                   size_t   cols,
                   const T* values, size_t nValues,
                   const size_t* rowOffsets,    size_t nRowOffsets,
                   const size_t* columnIndices, size_t nColumnIndices) 
                                             : Base(rows, cols) {
        this->_values = std::vector<T>(values, values + nValues);
        this->_rowOffsets = std::vector<size_t>(rowOffsets, rowOffsets + nRowOffsets);
        this->_columnIndices = std::vector<size_t>(columnIndices, columnIndices + nColumnIndices);
    }

    inline std::vector<T>& getRowOffsets() { return _rowOffsets; }
    inline std::vector<T>& getRowOffsets() const { return _rowOffsets; }
    inline void setRowOffsets(const std::vector<T>& rowOffsets) { _rowOffsets = rowOffsets; }
    inline std::vector<T>& getColumnIndices() { return _columnIndices; }
    inline std::vector<T>& getColumnIndices() const { return _columnIndices; }
    inline void setColumnIndices(const std::vector<T>& columnIndices) { _columnIndices = columnIndices; }
protected:

    friend Base;
    std::vector<T> _values; ///< Flat array for symmetric matrix data
    std::vector<size_t> _rowOffsets; ///< Represents the start of each row in the values array (size = rows + 1)
    std::vector<size_t> _columnIndices; ///< Represents the column index of each value in the values array

    inline std::vector<T> _values() { return _values; }

    inline const std::vector<T>& _values() const { return _values; }
};


#endif // CompressedSparseMATRIXACCESSOR_COMPRESSEDSPARSETSPECIALIZATION_H