#ifndef MATRIX_TYPES_H
#define MATRIX_TYPES_H

enum class MatrixStorageType {
    General,
    COO,
    CompressedSparse,
    Diagonal,
    None
};

enum class FormType {
    Full,
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

enum class MatrixSubstructureType {
    Row,
    Column,
    Diagonal,
    Block
};

#endif // MATRIX_TYPES_H