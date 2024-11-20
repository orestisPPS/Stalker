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
    Full,
    UpperTriangular,
    LowerTriangular,
    None
};

enum class OrderType {
    RowMajor,
    ColumnMajor,
    None
};

enum class SubstructureType {
    Row,
    Column,
    Diagonal,
    Block
};

#endif // MATRIX_TYPES_H