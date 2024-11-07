#ifndef MATRIX_DATA_ACCESSORS_H
#define MATRIX_DATA_ACCESSORS_H
#include <utility>
#include <type_traits>
#include "MatrixTypes.h"

// Base template for a full matrix
template <typename T, MatrixFormType formType, MatrixElementsOrder orderType, MatrixStorageType storageType>
class MatrixAccessor {
public:
    inline static constexpr T* element(T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            return matrix + i * numCols + j;
        } else { // ColumnMajor
            return matrix + j * numRows + i;
        }
    }
    
    inline static constexpr const T* element(const T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            return matrix + i * numCols + j;
        } else { // ColumnMajor
            return matrix + j * numRows + i;
        }
    }
};

// ===========================
// FULL MATRIX SPECIALIZATIONS
// ===========================

// Specialization for Symmetric Matrix
template <typename T, MatrixElementsOrder orderType>
class MatrixAccessor<T, MatrixFormType::Symmetric, orderType, MatrixStorageType::Full> {
public:
    inline static constexpr T* element(T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            return (i <= j) ? (matrix + i * numCols + j) : (matrix + j * numCols + i);
        } else { // ColumnMajor
            return (i <= j) ? (matrix + j * numRows + i) : (matrix + i * numRows + j);
        }
    }

    inline static constexpr const T* element(const T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            return (i <= j) ? (matrix + i * numCols + j) : (matrix + j * numCols + i);
        } else { // ColumnMajor
            return (i <= j) ? (matrix + j * numRows + i) : (matrix + i * numRows + j);
        }
    }
};

// Specialization for Upper Triangular Matrix
template <typename T, MatrixElementsOrder orderType>
class MatrixAccessor<T, MatrixFormType::UpperTriangular, orderType, MatrixStorageType::Full> {
public:
    inline static constexpr T* element(T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            // Return value for elements on or above the diagonal
            return (i <= j) ? (matrix + i * numCols + j) : nullptr;
        } else { // ColumnMajor
            return (i <= j) ? (matrix + j * numRows + i) : nullptr;
        }
    }

    inline static constexpr const T* element(const T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            return (i <= j) ? (matrix + i * numCols + j) : nullptr;
        } else { // ColumnMajor
            return (i <= j) ? (matrix + j * numRows + i) : nullptr;
        }
    }
};

// Specialization for Lower Triangular Matrix
template <typename T, MatrixElementsOrder orderType>
class MatrixAccessor<T, MatrixFormType::LowerTriangular, orderType, MatrixStorageType::Full> {
public:
    inline static constexpr T* element(T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            // Return value for elements on or below the diagonal
            return (i >= j) ? (matrix + i * numCols + j) : nullptr;
        } else { // ColumnMajor
            return (i >= j) ? (matrix + j * numRows + i) : nullptr;
        }
    }

    inline static constexpr const T* element(const T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {
            return (i >= j) ? (matrix + i * numCols + j) : nullptr;
        } else { // ColumnMajor
            return (i >= j) ? (matrix + j * numRows + i) : nullptr;
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
        std::vector<T>& values,
        const std::vector<unsigned int>& indices,
        const std::vector<unsigned int>& offsets,
        unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {

        if constexpr (orderType == MatrixElementsOrder::RowMajor) {  // CSR-like for symmetric
            if (i > j) std::swap(i, j);  // Always access the upper triangle
            if (i >= numRows) return nullptr;  // Out of bounds
            for (unsigned int idx = offsets[i]; idx < offsets[i + 1]; ++idx) {
                if (indices[idx] == j) {
                    return &values[idx];
                }
            }
        } else {  // ColumnMajor-like for symmetric (CSC)
            if (i > j) std::swap(i, j);  // Always access the upper triangle
            if (j >= numCols) return nullptr;  // Out of bounds
            for (unsigned int idx = offsets[j]; idx < offsets[j + 1]; ++idx) {
                if (indices[idx] == i) {
                    return &values[idx];
                }
            }
        }
        return nullptr;  // Zero if not found
    }

    inline static constexpr const T* element(
        const std::vector<T>& values,
        const std::vector<unsigned int>& indices,
        const std::vector<unsigned int>& offsets,
        unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {

        if constexpr (orderType == MatrixElementsOrder::RowMajor) {  // CSR-like for symmetric
            if (i > j) std::swap(i, j);  // Always access the upper triangle
            if (i >= numRows) return nullptr;  // Out of bounds
            for (unsigned int idx = offsets[i]; idx < offsets[i + 1]; ++idx) {
                if (indices[idx] == j) {
                    return &values[idx];
                }
            }
        } else {  // ColumnMajor-like for symmetric (CSC)
            if (i > j) std::swap(i, j);  // Always access the upper triangle
            if (j >= numCols) return nullptr;  // Out of bounds
            for (unsigned int idx = offsets[j]; idx < offsets[j + 1]; ++idx) {
                if (indices[idx] == i) {
                    return &values[idx];
                }
            }
        }
        return nullptr;  // Zero if not found
    }
};

// Specialization for Compressed Sparse Upper Triangular Matrix
template <typename T, MatrixElementsOrder orderType>
class MatrixAccessor<T, MatrixFormType::UpperTriangular, orderType, MatrixStorageType::CompressedSparse> {
public:
    inline static constexpr T* element(
        std::vector<T>& values,
        const std::vector<unsigned int>& indices,
        const std::vector<unsigned int>& offsets,
        unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {

        if (i > j) return nullptr;  // Below diagonal
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {  // CSR-like for upper triangular
            if (i >= numRows) return nullptr;
            for (unsigned int idx = offsets[i]; idx < offsets[i + 1]; ++idx) {
                if (indices[idx] == j) {
                    return &values[idx];
                }
            }
        } else {  // ColumnMajor-like (CSC) for upper triangular
            if (j >= numCols) return nullptr;
            for (unsigned int idx = offsets[j]; idx < offsets[j + 1]; ++idx) {
                if (indices[idx] == i) {
                    return &values[idx];
                }
            }
        }
        return nullptr;  // Zero if not found
    }

    inline static constexpr const T* element(
        const std::vector<T>& values,
        const std::vector<unsigned int>& indices,
        const std::vector<unsigned int>& offsets,
        unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {

        if (i > j) return nullptr;  // Below diagonal
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {  // CSR-like for upper triangular
            if (i >= numRows) return nullptr;
            for (unsigned int idx = offsets[i]; idx < offsets[i + 1]; ++idx) {
                if (indices[idx] == j) {
                    return &values[idx];
                }
            }
        } else {  // ColumnMajor-like (CSC) for upper triangular
            if (j >= numCols) return nullptr;
            for (unsigned int idx = offsets[j]; idx < offsets[j + 1]; ++idx) {
                if (indices[idx] == i) {
                    return &values[idx];
                }
            }
        }
        return nullptr;  // Zero if not found
    }
};

// Specialization for Compressed Sparse Lower Triangular Matrix
template <typename T, MatrixElementsOrder orderType>
class MatrixAccessor<T, MatrixFormType::LowerTriangular, orderType, MatrixStorageType::CompressedSparse> {
public:
    inline static constexpr T* element(
        std::vector<T>& values,
        const std::vector<unsigned int>& indices,
        const std::vector<unsigned int>& offsets,
        unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {

        if (i < j) return nullptr;  // Above diagonal
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {  // CSR-like for lower triangular
            if (i >= numRows) return nullptr;
            for (unsigned int idx = offsets[i]; idx < offsets[i + 1]; ++idx) {
                if (indices[idx] == j) {
                    return &values[idx];
                }
            }
        } else {  // ColumnMajor-like (CSC) for lower triangular
            if (j >= numCols) return nullptr;
            for (unsigned int idx = offsets[j]; idx < offsets[j + 1]; ++idx) {
                if (indices[idx] == i) {
                    return &values[idx];
                }
            }
        }
        return nullptr;  // Zero if not found
    }

    inline static constexpr const T* element(
        const std::vector<T>& values,
        const std::vector<unsigned int>& indices,
        const std::vector<unsigned int>& offsets,
        unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {

        if (i < j) return nullptr;  // Above diagonal
        if constexpr (orderType == MatrixElementsOrder::RowMajor) {  // CSR-like for lower triangular
            if (i >= numRows) return nullptr;
            for (unsigned int idx = offsets[i]; idx < offsets[i + 1]; ++idx) {
                if (indices[idx] == j) {
                    return &values[idx];
                }
            }
        } else {  // ColumnMajor-like (CSC) for lower triangular
            if (j >= numCols) return nullptr;
            for (unsigned int idx = offsets[j]; idx < offsets[j + 1]; ++idx) {
                if (indices[idx] == i) {
                    return &values[idx];
                }
            }
        }
        return nullptr;  // Zero if not found
    }
};
#endif // MATRIX_DATA_ACCESSORS_H
