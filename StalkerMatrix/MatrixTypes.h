#ifndef MATRIX_TYPES_H
#define MATRIX_TYPES_H

enum class MatrixStorageType {
    Full,
    COO,
    CompressedSparse,
    Diagonal,
    None
};

enum class MatrixFormType {
    Symmetric,
    NonSymmetric,
    UpperTriangular,
    LowerTriangular,
    None
};

enum class MatrixElementsOrder {
    RowMajor,
    ColumnMajor,
    None
};

#endif // MATRIX_TYPES_H