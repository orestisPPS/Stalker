#ifndef MATRIX_TYPES_H
#define MATRIX_TYPES_H

enum class MatrixStorageType {
    Dense,
    COO,
    CompressedSparse,
    Diagonal,
    None
};

enum class FormType {
    Symmetric,
    NonSymmetric,
    UpperTriangular,
    LowerTriangular,
    None
};

enum class OrderType {
    RowMajor,
    ColumnMajor,
    None
};

#endif // MATRIX_TYPES_H