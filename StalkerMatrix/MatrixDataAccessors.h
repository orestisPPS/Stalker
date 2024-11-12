#ifndef MATRIX_DATA_ACCESSORS_H
#define MATRIX_DATA_ACCESSORS_H
#include <utility>
#include <type_traits>
#include <stdexcept>
#include "MatrixTypes.h"
#include "MatrixBuffers.h"


// Base template for a full matrix
template <typename T, FormType FormT, OrderType OrderT, MatrixStorageType storageType>
class MatrixAccessor {

};

// ===========================
// FULL MATRIX SPECIALIZATIONS
// ===========================

// Specialization for Symmetric Matrix
template <typename T>
class MatrixAccessor<T, FormType::Symmetric, OrderType::RowMajor, MatrixStorageType::Dense> {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i <= j) ? (matrix + i * numCols + j) : (matrix + j * numCols + i);
    }
    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i <= j) ? (matrix + i * numCols + j) : (matrix + j * numCols + i);
    }
};

template <typename T>
class MatrixAccessor<T, FormType::Symmetric, OrderType::ColumnMajor, MatrixStorageType::Dense> {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i <= j) ? (matrix + j * numRows + i) : (matrix + i * numRows + j);
    }
    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i <= j) ? (matrix + j * numRows + i) : (matrix + i * numRows + j);
    }
};

// Specialization for Non-Symmetric Matrix
template <typename T>
class MatrixAccessor<T, FormType::NonSymmetric, OrderType::RowMajor, MatrixStorageType::Dense> {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return matrix + i * numCols + j;
    }
    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return matrix + i * numCols + j;
    }
};

template <typename T>
class MatrixAccessor<T, FormType::NonSymmetric, OrderType::ColumnMajor, MatrixStorageType::Dense> {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return matrix + j * numRows + i;
    }
    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return matrix + j * numRows + i;
    }
};

// Specialization for Upper Triangular Matrix
template <typename T>
class MatrixAccessor<T, FormType::UpperTriangular, OrderType::RowMajor, MatrixStorageType::Dense> {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i >= j) ? (matrix + i * numCols + j) : nullptr;
    }
    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i >= j) ? (matrix + i * numCols + j) : nullptr;
    }
};

template <typename T>
class MatrixAccessor<T, FormType::UpperTriangular, OrderType::ColumnMajor, MatrixStorageType::Dense> {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i >= j) ? (matrix + j * numRows + i) : nullptr;
    }
    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i >= j) ? (matrix + j * numRows + i) : nullptr;
    }
};

// Specialization for Lower Triangular Matrix
template <typename T>
class MatrixAccessor<T, FormType::LowerTriangular, OrderType::RowMajor, MatrixStorageType::Dense> {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i <= j) ? (matrix + i * numCols + j) : nullptr;
    }
    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i <= j) ? (matrix + i * numCols + j) : nullptr;
    }
};

template <typename T>
class MatrixAccessor<T, FormType::LowerTriangular, OrderType::ColumnMajor, MatrixStorageType::Dense> {
public:
    inline static constexpr T* element(T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i <= j) ? (matrix + j * numRows + i) : nullptr;
    }
    inline static constexpr const T* element(const T* matrix, unsigned i, unsigned j, unsigned numRows, unsigned numCols) {
        return (i <= j) ? (matrix + j * numRows + i) : nullptr;
    }
};

// ========================================
// COMPRESSED SPARSE MATRIX SPECIALIZATIONS
// ========================================
// Specialization for Compressed Sparse Symmetric Matrix
template <typename T, OrderType OrderT>
class MatrixAccessor<T, FormType::Symmetric, OrderT, MatrixStorageType::CompressedSparse> {
public:
    inline static constexpr T* element(
        T * values,
        const unsigned *indices,
        const unsigned *offsets,
        unsigned i, unsigned j, unsigned numRows, unsigned numCols) {

        if (i > j) std::swap(i, j);  // Always access the upper triangle
        if constexpr (OrderT == OrderType::RowMajor) // CSR-like for symmetric
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
        if constexpr (OrderT == OrderType::RowMajor)  // CSR-like for symmetric
            return _getCompressedElement(values, indices, offsets, i, j, numRows, numCols);
        else  // ColumnMajor-like for symmetric (CSC)
            return _getCompressedElement(values, indices, offsets, j, i, numCols, numRows);
        return nullptr;  // Zero if not found
    }
};

// Specialization for Compressed Sparse Upper Triangular Matrix
template <typename T, OrderType OrderT>
class MatrixAccessor<T, FormType::UpperTriangular, OrderT, MatrixStorageType::CompressedSparse> {
public:
    inline static constexpr T* element(
        T * values,
        const unsigned *indices,
        const unsigned *offsets,
        unsigned i, unsigned j, unsigned numRows, unsigned numCols) {

        if (i > j) return nullptr;  // Below diagonal
        if constexpr (OrderT == OrderType::RowMajor) {  // CSR-like for upper triangular
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
        if constexpr (OrderT == OrderType::RowMajor) {  // CSR-like for upper triangular
            return _getCompressedElement(values, indices, offsets, i, j, numRows, numCols);
        } else {  // ColumnMajor-like (CSC) for upper triangular
            return _getCompressedElement(values, indices, offsets, j, i, numCols, numRows);
        }
        return nullptr;  // Zero if not found
    }
};

// Specialization for Compressed Sparse Lower Triangular Matrix
template <typename T, OrderType OrderT>
class MatrixAccessor<T, FormType::LowerTriangular, OrderT, MatrixStorageType::CompressedSparse> {
public:
    inline static constexpr T* element(
        T * values,
        const unsigned *indices,
        const unsigned *offsets,
        unsigned i, unsigned j, unsigned numRows, unsigned numCols) {

        if (i < j) return nullptr;  // Above diagonal
        if constexpr (OrderT == OrderType::RowMajor) // CSR-like for lower triangular
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
        if constexpr (OrderT == OrderType::RowMajor)  // CSR-like for lower triangular
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



#endif // MATRIX_DATA_ACCESSORS_H
