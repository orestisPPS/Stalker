/**
 * @file MatrixTypes.h
 * @brief Defines various enumerations for matrix storage layouts, form types, order types, and substructure types.
 * 
 * This header file contains enumerations that describe different properties and structures of matrices.
 * These enumerations are used to specify how matrices are stored, their structural forms, the order in which
 * elements are stored, and the types of substructures they may contain.
 */
#ifndef MATRIX_TYPES_H
#define MATRIX_TYPES_H


/**
 * @enum MatrixStorageLayout
 * @brief Specifies the storage layout of a matrix.
 * 
 * This enumeration defines the different ways a matrix can be stored in memory.
 * - Dense: Standard dense matrix storage.
 * - CompressedSparse: Compressed sparse row or column storage.
 * - COO: Coordinate list storage.
 * - None: No specific storage layout.
 */
enum class MatrixStorageLayout {
    Dense,              ///< Dense matrix storage.
    CompressedSparse,   ///< Compressed sparse row or column storage.
    COO,                ///< Coordinate list storage.
    None                ///< No specific storage layout.
};

/**
 * @enum FormType
 * @brief Specifies the form or structure of a matrix.
 * 
 * This enumeration defines the structural form of a matrix.
 * - Full: Full matrix with no specific structure.
 * - Symmetric: Symmetric matrix where elements are mirrored across the diagonal.
 * - UpperTriangular: Upper triangular matrix with non-zero elements only above the diagonal.
 * - LowerTriangular: Lower triangular matrix with non-zero elements only below the diagonal.
 * - Diagonal: Diagonal matrix with non-zero elements only on the diagonal.
 * - None: No specific form.
 */
enum class FormType {
    Full,
    Symmetric,          ///< Symmetric matrix.
    UpperTriangular,    ///< Upper triangular matrix.
    LowerTriangular,    ///< Lower triangular matrix.
    Diagonal,           ///< Diagonal matrix storage.
    None                ///< No specific form.
};

/**
 * @enum OrderType
 * @brief Specifies the storage order of a matrix.
 * 
 * This enumeration defines the order in which matrix elements are stored in memory.
 * - RowMajor: Row-major order where elements of a row are stored contiguously.
 * - ColumnMajor: Column-major order where elements of a column are stored contiguously.
 * - None: No specific order.
 */
enum class OrderType {
    RowMajor,           ///< Row-major order.
    ColumnMajor,        ///< Column-major order.
    None                ///< No specific order.
};

/**
 * @enum MatrixRegionType
 * @brief Specifies the substructure type within a matrix.
 * 
 * This enumeration defines the types of substructures that can exist within a matrix.
 * - Row: Row substructure.
 * - Column: Column substructure.
 * - Diagonal: Diagonal substructure.
 * - Block: Block substructure.
 */
enum class RegionType {
    Row,                ///< Row substructure.
    Column,             ///< Column substructure.
    Diagonal,           ///< Diagonal substructure.
    Block               ///< Block substructure.
};

#endif // MATRIX_TYPES_H