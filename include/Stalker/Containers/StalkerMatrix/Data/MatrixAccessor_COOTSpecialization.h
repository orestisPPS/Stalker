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

#ifndef MATRIX_ACCESSOR_COO_T_SPECIALIZATION_H
#define MATRIX_ACCESSOR_COO_T_SPECIALIZATION_H
#include "DenseMatrixAccessor_FullTSpecialization.h"
#include <unordered_map>

template <typename T, T_Form FormT, OrderType OrderT>
class MatrixAccessor<T, StorageLayout::COO, FormT, OrderT> : public MatrixAccessorBase<MatrixAccessor<T, StorageLayout::COO, FormT, OrderT>, T, StorageLayout::COO, FormT, OrderT> {
    
    using Base = MatrixAccessorBase<MatrixAccessor<T, StorageLayout::COO, FormT, OrderT>, T, StorageLayout::COO, FormT, OrderT>;
    using RowBuffer = MatrixPtrBuffer<T, StorageLayout::COO, FormT, OrderT, RegionType::Row>;
    using ConstRowBuffer = MatrixPtrBuffer<const T, StorageLayout::COO, FormT, OrderT, RegionType::Row>;
    using ColumnBuffer = MatrixPtrBuffer<T, StorageLayout::COO, FormT, OrderT, RegionType::Column>;
    using ConstColumnBuffer = MatrixPtrBuffer<const T, StorageLayout::COO, FormT, OrderT, RegionType::Column>;

public:

    MatrixAccessor(size_t rows, size_t cols) : Base(rows, cols){
        values = std::unordered_map<std::pair<size_t, size_t>, T>();
        values.reserve(rows * cols);
    }
    MatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * cols, value) {}
    MatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize) {
            // checkSize(numValues, rows * cols, "MatrixAccessor:: Constructor");  
        }
    }

protected:

    friend Base;
    std::unordered_map<std::pair<size_t, size_t>, T> values; ///< Flat array for symmetric matrix data
};

#endif // MATRIX_ACCESSOR_COO_T_SPECIALIZATION_H