#ifndef MATRIX_DATA_ACCESSORS_H
#define MATRIX_DATA_ACCESSORS_H
#include <utility>
#include <type_traits>
#include <stdexcept>
#include "MatrixTypes.h"


template <typename T>
struct MatrixDataPtrBuffer {
    T* data_start;
    unsigned size;
};

// Base template for a full matrix
template <typename T, MatrixFormType formType, MatrixElementsOrder orderType, MatrixStorageType storageType>
class MatrixAccessor {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            return matrix + i * numCols + j;
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return matrix + j * numRows + i;
    }
    
    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            return matrix + i * numCols + j;
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return matrix + j * numRows + i;
    }

    inline static constexpr const void row(const T* matrix, unsigned numRows, unsigned numCols, unsigned row, MatrixDataPtrBuffer<T>& buffer) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            _getFullMatrixPtrBuffer(matrix, row, numCols, buffer);
         else if constexpr (orderType == MatrixElementsOrder::ColumnMajor) {
            throw std::invalid_argument("Raw pointer row access is not supported for ColumnMajor order.");
        }
    }

    inline static constexpr const void column(const T* matrix, unsigned numRows, unsigned numCols, MatrixDataPtrBuffer<T>& buffer, unsigned col) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            throw std::invalid_argument("Raw pointer column access is not supported for RowMajor order.");
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            _getFullMatrixPtrBuffer(matrix, col, numRows, buffer);
    }
};

// ===========================
// FULL MATRIX SPECIALIZATIONS
// ===========================

// Specialization for Symmetric Matrix
template <typename T, MatrixElementsOrder orderType>
class MatrixAccessor<T, MatrixFormType::Symmetric, orderType, MatrixStorageType::Full> {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            return (i <= j) ? (matrix + i * numCols + j) : (matrix + j * numCols + i);
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return (i <= j) ? (matrix + j * numRows + i) : (matrix + i * numRows + j);
    }

    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            return (i <= j) ? (matrix + i * numCols + j) : (matrix + j * numCols + i);
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return (i <= j) ? (matrix + j * numRows + i) : (matrix + i * numRows + j);
    }

    inline static constexpr const void row(const T* matrix, unsigned numRows, unsigned numCols, MatrixDataPtrBuffer<T>& buffer, unsigned row) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            _getFullMatrixPtrBuffer(matrix, row, numCols - row, buffer);
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            throw std::invalid_argument("Raw pointer row access is not supported for ColumnMajor order.");
    }

    inline static constexpr const void column(const T* matrix, unsigned numRows, unsigned numCols, MatrixDataPtrBuffer<T>& buffer, unsigned col) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            throw std::invalid_argument("Raw pointer column access is not supported for RowMajor order.");
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            _getFullMatrixPtrBuffer(matrix, col, numRows - col, buffer);
    }
};

// Specialization for Upper Triangular Matrix
template <typename T, MatrixElementsOrder orderType>
class MatrixAccessor<T, MatrixFormType::UpperTriangular, orderType, MatrixStorageType::Full> {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            // Return value for elements on or above the diagonal
            return (i <= j) ? (matrix + i * numCols + j) : nullptr;
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return (i <= j) ? (matrix + j * numRows + i) : nullptr;
    }

    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            return (i <= j) ? (matrix + i * numCols + j) : nullptr;
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return (i <= j) ? (matrix + j * numRows + i) : nullptr;
    }

    inline static constexpr const void row(const T* matrix, unsigned numRows, unsigned numCols, MatrixDataPtrBuffer<T>& buffer, unsigned row) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            // Start from the diagonal element (row, row), size is numCols - row
            buffer.data_start = const_cast<T*>(matrix) + row * numCols + row;  // Cast needed since matrix is const
            buffer.size = numCols - row;
        } else if constexpr (orderType == MatrixElementsOrder::ColumnMajor) {
            throw std::invalid_argument("Raw pointer row access is not supported for ColumnMajor order.");
        }
    }

    inline static constexpr const void column(const T* matrix, unsigned numRows, unsigned numCols, MatrixDataPtrBuffer<T>& buffer, unsigned col) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            throw std::invalid_argument("Raw pointer column access is not supported for RowMajor order.");
        } else if constexpr (orderType == MatrixElementsOrder::ColumnMajor) {
            // Start from the diagonal element (col, col), size is numRows - col
            buffer.data_start = const_cast<T*>(matrix) + col * numRows + col;  // Cast needed since matrix is const
            buffer.size = numRows - col;
        }
    }

    
};

// Specialization for Lower Triangular Matrix
template <typename T, MatrixElementsOrder orderType>
class MatrixAccessor<T, MatrixFormType::LowerTriangular, orderType, MatrixStorageType::Full> {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            // Return value for elements on or below the diagonal
            return (i >= j) ? (matrix + i * numCols + j) : nullptr;
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return (i >= j) ? (matrix + j * numRows + i) : nullptr;
    }

    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor)
            return (i >= j) ? (matrix + i * numCols + j) : nullptr;
        else if constexpr (orderType == MatrixElementsOrder::ColumnMajor)
            return (i >= j) ? (matrix + j * numRows + i) : nullptr;
    }

    inline static constexpr const void row(const T* matrix, unsigned numRows, unsigned numCols, MatrixDataPtrBuffer<T>& buffer, unsigned row) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            // Start from the first element in the row (row, 0), size is row + 1
            buffer.data_start = const_cast<T*>(matrix) + row * numCols;
            buffer.size = row + 1;
        } else if constexpr (orderType == MatrixElementsOrder::ColumnMajor) {
            throw std::invalid_argument("Raw pointer row access is not supported for ColumnMajor order.");
        }
    }

    inline static constexpr const void column(const T* matrix, unsigned numRows, unsigned numCols, MatrixDataPtrBuffer<T>& buffer, unsigned col) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            throw std::invalid_argument("Raw pointer column access is not supported for RowMajor order.");
        } else if constexpr (orderType == MatrixElementsOrder::ColumnMajor) {
            // Start from the first element in the column (0, col), size is col + 1
            buffer.data_start = const_cast<T*>(matrix) + col;
            buffer.size = col + 1;
        }
    }


};

// ========================================
// COMPRESSED SPARSE MATRIX SPECIALIZATIONS
// ========================================
// Specialization for Compressed Sparse Symmetric Matrix
template <typename T, MatrixElementsOrder orderType>
class MatrixAccessor<T, MatrixFormType::Symmetric, orderType, MatrixStorageType::CompressedSparse> {
public:
    inline static constexpr T* element(
        T * values,
        const unsigned *indices,
        const unsigned *offsets,
        unsigned i, unsigned j, unsigned numRows, unsigned numCols) {

        if (i > j) std::swap(i, j);  // Always access the upper triangle
        if constexpr (orderType == MatrixElementsOrder::RowMajor) // CSR-like for symmetric
            return _getCompressedElement(values, indices, offsets, i, j, numRows, numCols);
        else  // ColumnMajor-like for symmetric (CSC)
            return _getCompressedElement(values, indices, offsets, j, i, numCols, numRows);
        return nullptr;  // Zero if not found
    }

    inline static constexpr const T* element(
        T * values,
        const unsigned *indices,
        const unsigned *offsets,
        unsigned i, unsigned j, unsigned numRows, unsigned numCols) {

        if (i > j) std::swap(i, j);  // Always access the upper triangle
        if constexpr (orderType == MatrixElementsOrder::RowMajor)  // CSR-like for symmetric
            return _getCompressedElement(values, indices, offsets, i, j, numRows, numCols);
        else  // ColumnMajor-like for symmetric (CSC)
            return _getCompressedElement(values, indices, offsets, j, i, numCols, numRows);
        return nullptr;  // Zero if not found
    }
};

// Specialization for Compressed Sparse Upper Triangular Matrix
template <typename T, MatrixElementsOrder orderType>
class MatrixAccessor<T, MatrixFormType::UpperTriangular, orderType, MatrixStorageType::CompressedSparse> {
public:
    inline static constexpr T* element(
        T * values,
        const unsigned *indices,
        const unsigned *offsets,
        unsigned i, unsigned j, unsigned numRows, unsigned numCols) {

        if (i > j) return nullptr;  // Below diagonal
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {  // CSR-like for upper triangular
            return _getCompressedElement(values, indices, offsets, i, j, numRows, numCols);
        } else {  // ColumnMajor-like (CSC) for upper triangular
            return _getCompressedElement(values, indices, offsets, j, i, numCols, numRows);
        }
        return nullptr;  // Zero if not found
    }

    inline static constexpr const T* element(
        T * values,
        const unsigned *indices,
        const unsigned *offsets,
        unsigned i, unsigned j, unsigned numRows, unsigned numCols) {

        if (i > j) return nullptr;  // Below diagonal
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {  // CSR-like for upper triangular
            return _getCompressedElement(values, indices, offsets, i, j, numRows, numCols);
        } else {  // ColumnMajor-like (CSC) for upper triangular
            return _getCompressedElement(values, indices, offsets, j, i, numCols, numRows);
        }
        return nullptr;  // Zero if not found
    }
};

// Specialization for Compressed Sparse Lower Triangular Matrix
template <typename T, MatrixElementsOrder orderType>
class MatrixAccessor<T, MatrixFormType::LowerTriangular, orderType, MatrixStorageType::CompressedSparse> {
public:
    inline static constexpr T* element(
        T * values,
        const unsigned *indices,
        const unsigned *offsets,
        unsigned i, unsigned j, unsigned numRows, unsigned numCols) {

        if (i < j) return nullptr;  // Above diagonal
        if constexpr (orderType == MatrixElementsOrder::RowMajor) // CSR-like for lower triangular
            return _getCompressedElement(values, indices, offsets, i, j, numRows, numCols);
        else  // ColumnMajor-like (CSC) for lower triangular
            return _getCompressedElement(values, indices, offsets, j, i, numCols, numRows);
        return nullptr;  // Zero if not found
    }

    inline static constexpr const T* element(
        T * values,
        const unsigned *indices,
        const unsigned *offsets,
        unsigned i, unsigned j, unsigned numRows, unsigned numCols) {

        if (i < j) return nullptr;  // Above diagonal
        if constexpr (orderType == MatrixElementsOrder::RowMajor)  // CSR-like for lower triangular
            return _getCompressedElement(values, indices, offsets, i, j, numRows, numCols);
        else  // ColumnMajor-like (CSC) for lower triangular
            return _getCompressedElement(values, indices, offsets, j, i, numCols, numRows);
        return nullptr;  // Zero if not found
    }
};

template<typename T>
inline static constexpr T* _getCompressedElement(
    T* values,
    const unsigned *indices,
    const unsigned *offsets,
    unsigned i, unsigned j, unsigned numRows, unsigned numCols) {

    for (unsigned idx = offsets[i]; idx < offsets[i + 1]; ++idx)
        if (indices[idx] == j)
            return &values[idx];
    return nullptr;  // Zero if not found
}

template<typename T>
inline static constexpr const T* _getCompressedElement(
    T* values,
    const unsigned *indices,
    const unsigned *offsets,
    unsigned i, unsigned j, unsigned numRows, unsigned numCols) {

    for (unsigned idx = offsets[i]; idx < offsets[i + 1]; ++idx)
        if (indices[idx] == j)
            return &values[idx];
    return nullptr;  // Zero if not found
}

template<typename T>
inline static constexpr void _getFullMatrixPtrBuffer(T* matrix, unsigned i, unsigned n, MatrixDataPtrBuffer<T>& buffer) {
    buffer.data_start = matrix + i * n;
    buffer.size = n;
}

#endif // MATRIX_DATA_ACCESSORS_H
