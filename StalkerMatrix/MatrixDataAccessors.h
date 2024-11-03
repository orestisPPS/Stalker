#ifndef MATRIX_DATA_ACCESSORS_H
#define MATRIX_DATA_ACCESSORS_H


/**
 * @brief Base class for matrix accessors.
 * 
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class MatrixAccessorBase { };

/**
 * @brief Pure virtual base class for full matrix accessors.
 * 
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class FullMatrixAccessorBase : public MatrixAccessorBase<T> {
public:
    virtual T* operator()(T* matrix, unsigned int i, unsigned int j, unsigned int numCols) = 0;
    virtual const T* operator()(const T* matrix, unsigned int i, unsigned int j, unsigned int numCols) const = 0;
};

/**
 * @brief Pure virtual base class for compressed sparse matrix accessors.
 * 
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class CompressedSparseMatrixAccessorBase {
public:
    virtual T* operator() = 0;
    virtual const T* operator() const = 0;
};



/**
 * @brief Accessor for row-major non-symmetric full matrices.
 * 
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class RowMajorNonSymmetricFullMatrixAccessor : public FullMatrixAccessorBase<T> {
public:
    /**
     * @brief Accesses an element in a row-major non-symmetric full matrix.
     * 
     * @param matrix Pointer to the matrix data.
     * @param i Row index.
     * @param j Column index.
     * @param numCols Number of columns in the matrix.
     * @return T* Pointer to the matrix element.
     */
    static inline T* operator()(T* matrix, unsigned int i, unsigned int j, unsigned int numCols) override
        return matrix + i * numCols + j;

    /**
     * @brief Accesses an element in a row-major non-symmetric full matrix.
     * 
     * @param matrix Pointer to the matrix data.
     * @param i Row index.
     * @param j Column index.
     * @param numCols Number of columns in the matrix.
     * @return const T* Pointer to the matrix element.
     */
    static inline const T* operator()(const T* matrix, unsigned int i, unsigned int j, unsigned int numCols) const override
        return matrix + i * numCols + j;

};

/**
 * @brief Accessor for row-major symmetric full matrices.
 * 
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class RowMajorSymmetricFullMatrixAccessor : public FullMatrixAccessorBase<T> {
public:
    /**
     * @brief Accesses an element in a row-major symmetric full matrix.
     * 
     * @param matrix Pointer to the matrix data.
     * @param i Row index.
     * @param j Column index.
     * @param numCols Number of columns in the matrix.
     * @return T* Pointer to the matrix element.
     */
    static inline T* operator()(T* matrix, unsigned int i, unsigned int j, unsigned int numCols) override {
        if (i < j)
            return matrix + i * numCols + j;
        else
            return matrix + j * numCols + i;
    }

    /**
     * @brief Accesses an element in a row-major symmetric full matrix.
     * 
     * @param matrix Pointer to the matrix data.
     * @param i Row index.
     * @param j Column index.
     * @param numCols Number of columns in the matrix.
     * @return const T* Pointer to the matrix element.
     */
    static inline const T* operator()(const T* matrix, unsigned int i, unsigned int j, unsigned int numCols) const override {
        if (i < j)
            return matrix + i * numCols + j;
        else
            return matrix + j * numCols + i;
    }
};

/**
 * @brief Accessor for row-major upper triangular full matrices.
 * 
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class RowMajorUpperTriangularFullMatrixAccessor : public RowMajorSymmetricFullMatrixAccessor<T> { };

/**
 * @brief Accessor for row-major lower triangular full matrices.
 * 
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class RowMajorLowerTriangularFullMatrixAccessor : public RowMajorSymmetricFullMatrixAccessor<T> { };

/**
 * @brief Accessor for column-major non-symmetric full matrices.
 * 
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class ColumnMajorNonSymmetricFullMatrixAccessor : public FullMatrixAccessorBase<T> {
public:
    /**
     * @brief Accesses an element in a column-major non-symmetric full matrix.
     * 
     * @param matrix Pointer to the matrix data.
     * @param i Row index.
     * @param j Column index.
     * @param numRows Number of rows in the matrix.
     * @return T* Pointer to the matrix element.
     */
    static inline T* operator()(T* matrix, unsigned int i, unsigned int j, unsigned int numRows) override {
        return matrix + j * numRows + i;
    }

    /**
     * @brief Accesses an element in a column-major non-symmetric full matrix.
     * 
     * @param matrix Pointer to the matrix data.
     * @param i Row index.
     * @param j Column index.
     * @param numRows Number of rows in the matrix.
     * @return const T* Pointer to the matrix element.
     */
    static inline const T* operator()(const T* matrix, unsigned int i, unsigned int j, unsigned int numRows) const override {
        return matrix + j * numRows + i;
    }
};

/**
 * @brief Accessor for column-major symmetric full matrices.
 * 
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class ColumnMajorSymmetricFullMatrixAccessor : public FullMatrixAccessorBase<T> {
public:
    /**
     * @brief Accesses an element in a column-major symmetric full matrix.
     * 
     * @param matrix Pointer to the matrix data.
     * @param i Row index.
     * @param j Column index.
     * @param numRows Number of rows in the matrix.
     * @return T* Pointer to the matrix element.
     */
    static inline T* operator()(T* matrix, unsigned int i, unsigned int j, unsigned int numRows) override {
        if (i < j)
            return matrix + j * numRows + i;
        else
            return matrix + i * numRows + j;
    }

    /**
     * @brief Accesses an element in a column-major symmetric full matrix.
     * 
     * @param matrix Pointer to the matrix data.
     * @param i Row index.
     * @param j Column index.
     * @param numRows Number of rows in the matrix.
     * @return const T* Pointer to the matrix element.
     */
    static inline const T* operator()(const T* matrix, unsigned int i, unsigned int j, unsigned int numRows) const override {
        if (i < j)
            return matrix + j * numRows + i;
        else
            return matrix + i * numRows + j;
    }
};

/**
 * @brief Accessor for column-major upper triangular full matrices.
 * 
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class ColumnMajorUpperTriangularFullMatrixAccessor : public ColumnMajorSymmetricFullMatrixAccessor<T> { };

/**
 * @brief Accessor for column-major lower triangular full matrices.
 * 
 * @tparam T Type of the matrix elements.
 */
template <typename T>
class ColumnMajorLowerTriangularFullMatrixAccessor : public ColumnMajorSymmetricFullMatrixAccessor<T> { };

#endif // MATRIX_DATA_ACCESSORS_H