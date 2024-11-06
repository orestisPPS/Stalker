#ifndef MATRIX_DATA_ACCESSORS_H
#define MATRIX_DATA_ACCESSORS_H

#include <utility>
#include <type_traits>
#include "MatrixTypes.h"


template <typename T, MatrixFormType formType, MatrixElementsOrder order>
class MatrixAccessor {
public:
    static constexpr T* element(T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        // Full matrix indexing with row-major or column-major ordering
        return (order == MatrixElementsOrder::RowMajor)
            ? (matrix + i * numCols + j)
            : (matrix + j * numRows + i);
    }
    
    static constexpr const T* element(const T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        return (order == MatrixElementsOrder::RowMajor)
            ? (matrix + i * numCols + j)
            : (matrix + j * numRows + i);
    }
};

template <typename T, MatrixElementsOrder order>
class MatrixAccessor<T, MatrixFormType::Symmetric, order> {
public:
    static constexpr T* element(T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if constexpr (order == MatrixElementsOrder::RowMajor)
            return (i <= j) ? (matrix + i * numCols + j) : (matrix + j * numCols + i);
        else if constexpr (order == MatrixElementsOrder::ColumnMajor)
            return (i <= j) ? (matrix + j * numRows + i) : (matrix + i * numRows + j);
    }

    static constexpr const T* element(const T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if (order == MatrixElementsOrder::RowMajor)
            return (i <= j) ? (matrix + i * numCols + j) : (matrix + j * numCols + i);
        else if constexpr (order == MatrixElementsOrder::ColumnMajor)
            return (i <= j) ? (matrix + j * numRows + i) : (matrix + i * numRows + j);
    }
};

template <typename T, MatrixElementsOrder order>
class MatrixAccessor<T, MatrixFormType::UpperTriangular, order> {
public:
    static constexpr T* element(T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if (i > j) {
            return nullptr; // Elements below the diagonal do not exist
        }
        return (order == MatrixElementsOrder::RowMajor)
            ? (matrix + i * numCols + j)
            : (matrix + j * numRows + i);
    }

    static constexpr const T* element(const T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if (i > j) {
            return nullptr; // Elements below the diagonal do not exist
        }
        return (order == MatrixElementsOrder::RowMajor)
            ? (matrix + i * numCols + j)
            : (matrix + j * numRows + i);
    }
};

template <typename T, MatrixElementsOrder order>
class MatrixAccessor<T, MatrixFormType::UpperTriangular, order> {
public:
    static constexpr T* element(T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if (i > j) {
            return nullptr; // Elements below the diagonal do not exist
        }
        return (order == MatrixElementsOrder::RowMajor)
            ? (matrix + i * numCols + j)
            : (matrix + j * numRows + i);
    }

    static constexpr const T* element(const T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if (i > j) {
            return nullptr; // Elements below the diagonal do not exist
        }
        return (order == MatrixElementsOrder::RowMajor)
            ? (matrix + i * numCols + j)
            : (matrix + j * numRows + i);
    }
};

template <typename T, MatrixElementsOrder order>
class MatrixAccessor<T, MatrixFormType::LowerTriangular, order> {
public:
    static constexpr T* element(T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if (i < j) {
            return nullptr; // Elements above the diagonal do not exist
        }
        return (order == MatrixElementsOrder::RowMajor)
            ? (matrix + i * numCols + j)
            : (matrix + j * numRows + i);
    }

    static constexpr const T* element(const T* matrix, unsigned int i, unsigned int j, unsigned int numRows, unsigned int numCols) {
        if (i < j) {
            return nullptr; // Elements above the diagonal do not exist
        }
        return (order == MatrixElementsOrder::RowMajor)
            ? (matrix + i * numCols + j)
            : (matrix + j * numRows + i);
    }
};






#endif // MATRIX_DATA_ACCESSORS_H
