#ifndef DENSEMATRIXACCESSOR_TRIANGULARTSPECIALIZATION_H
#define DENSEMATRIXACCESSOR_TRIANGULARTSPECIALIZATION_H


#include "DenseMatrixAccessor_FullTSpecialization.h"
//=================================================================================================
// ===================== Upper Triangular Row Major Matrix Template Specialization ================
//=================================================================================================
template <typename T>
class DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::RowMajor>
    : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::RowMajor>, T, FormType::UpperTriangular, OrderType::RowMajor> {

    using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::RowMajor>, T, FormType::UpperTriangular, OrderType::RowMajor>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize) {
            checkSize(numValues, rows * (rows + 1) / 2, "DenseMatrixAccessor:: Constructor");  
        }
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values; ///< Flat array for symmetric matrix data

    inline T& _element(size_t i, size_t j) {
        return values[i * (2 * this->_rows - i + 1) / 2 + (j - i)];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[i * (2 * this->_rows - i + 1) / 2 + (j - i)];
    }
    inline GeneralMatrixPtrBuffer<T> _row(size_t row) {
        return GeneralMatrixPtrBuffer<T>(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<const T> _row(size_t row) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<T> _column(size_t col) {
        return GeneralMatrixPtrBuffer<T>(values.data() + col, col + 1);
    }
    inline GeneralMatrixPtrBuffer<const T> _column(size_t col) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + col, col + 1);
    }
};
//=================================================================================================
// ===================== Upper Triangular Col Major Matrix Template Specialization ================
//=================================================================================================
template <typename T>
class DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::ColumnMajor>
    : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::ColumnMajor>, T, FormType::UpperTriangular, OrderType::ColumnMajor> {

    using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::ColumnMajor>, T, FormType::UpperTriangular, OrderType::ColumnMajor>;
public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize)
            checkSize(numValues, rows * (rows + 1) / 2, "DenseMatrixAccessor:: Constructor");  
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values;

    inline T& _element(size_t i, size_t j) {
        return values[j * (2 * this->_rows - j + 1) / 2 + i - j];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[j * (2 * this->_rows - j + 1) / 2 + i - j];
    }
    inline GeneralMatrixPtrBuffer<T> _row(size_t row) {
        return GeneralMatrixPtrBuffer<T>(values.data() + (row * (row + 1)) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<const T> _row(size_t row) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + (row * (row + 1)) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<T> _column(size_t col) {
        return GeneralMatrixPtrBuffer<T>(values.data() + (col * (col + 1)) / 2, col + 1);
    }
    inline GeneralMatrixPtrBuffer<const T> _column(size_t col) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + (col * (col + 1)) / 2, col + 1);
    }
};

//=================================================================================================
// ===================== Lower Triangular Row Major Matrix Template Specialization ================
//=================================================================================================
template <typename T>
class DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::RowMajor>
    : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::RowMajor>, T, FormType::LowerTriangular, OrderType::RowMajor> {

    using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::RowMajor>, T, FormType::LowerTriangular, OrderType::RowMajor>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize) {
            checkSize(numValues, rows * (rows + 1) / 2, "DenseMatrixAccessor:: Constructor");  
        }
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values; ///< Flat array for symmetric matrix data

    inline T& _element(size_t i, size_t j) {
        return values[i * (2 * this->_rows - i + 1) / 2 + (j - i)];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[i * (2 * this->_rows - i + 1) / 2 + (j - i)];
    }
    inline GeneralMatrixPtrBuffer<T> _row(size_t row) {
        return GeneralMatrixPtrBuffer<T>(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<const T> _row(size_t row) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + row * (2 * this->_rows - row + 1) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<T> _column(size_t col) {
        return GeneralMatrixPtrBuffer<T>(values.data() + col, col + 1);
    }
    inline GeneralMatrixPtrBuffer<const T> _column(size_t col) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + col, col + 1);
    }
};
//=================================================================================================
// ===================== Lower Triangular Col Major Matrix Template Specialization ================
//=================================================================================================
template <typename T>
class DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::ColumnMajor>
    : public MatrixAccessorBase<DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::ColumnMajor>, T, FormType::LowerTriangular, OrderType::ColumnMajor> {

    using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::ColumnMajor>, T, FormType::LowerTriangular, OrderType::ColumnMajor>;
public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize)
            checkSize(numValues, rows * (rows + 1) / 2, "DenseMatrixAccessor:: Constructor");  
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend Base;
    std::vector<T> values;

    inline T& _element(size_t i, size_t j) {
        return values[j * (2 * this->_rows - j + 1) / 2 + i - j];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[j * (2 * this->_rows - j + 1) / 2 + i - j];
    }
    inline GeneralMatrixPtrBuffer<T> _row(size_t row) {
        return GeneralMatrixPtrBuffer<T>(values.data() + (row * (row + 1)) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<const T> _row(size_t row) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + (row * (row + 1)) / 2, this->_rows - row);
    }
    inline GeneralMatrixPtrBuffer<T> _column(size_t col) {
        return GeneralMatrixPtrBuffer<T>(values.data() + (col * (col + 1)) / 2, col + 1);
    }
    inline GeneralMatrixPtrBuffer<const T> _column(size_t col) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + (col * (col + 1)) / 2, col + 1);
    }
};


// // ==============================
// // General Matrix: Upper Triangular
// // ==============================
// template <typename T, OrderType OrderT>
// class DenseMatrixAccessor<T, FormType::UpperTriangular, OrderT>
//     : public MatrixDataBase<DenseMatrixAccessor<T, FormType::UpperTriangular, OrderT>, T, FormType::UpperTriangular, OrderT> {

// protected:
//     friend class MatrixDataBase<DenseMatrixAccessor<T, FormType::UpperTriangular, OrderT>, T, FormType::UpperTriangular, OrderT>;
//     inline T& _element(size_t i, size_t j) {
//         if (i > j) throw std::out_of_range("Accessing non-stored element in upper triangular matrix.");
//         return values[(i * (2 * this->_rows - i + 1)) / 2];
//     }

//     inline const T& _element(size_t i, size_t j) const {
//         if (i > j) throw std::out_of_range("Accessing non-stored element in upper triangular matrix.");
//         return values[(i * (2 * this->_rows - i + 1)) / 2];
//     }

//     inline auto _row(size_t row) {
//         if constexpr (OrderT == OrderType::RowMajor)
//             return FixedStridePtrBuffer<T>(values.data() + (row * (2 * this->_rows - row + 1)) / 2, this->_cols - row);
//         else if constexpr (OrderT == OrderType::ColumnMajor)
//             return NonContiguousBlockPtrBuffer<T>(values.data() + (row * (row + 1)) / 2, this->_cols - row, this->_rows);
//     }
    
//     inline auto _row(size_t row) const {
//         if constexpr (OrderT == OrderType::RowMajor)
//             return FixedStridePtrBuffer<const T>(values.data() + (row * (2 * this->_rows - row + 1)) / 2, this->_cols - row);
//         else if constexpr (OrderT == OrderType::ColumnMajor)
//             return NonContiguousBlockPtrBuffer<const T>(values.data() + (row * (row + 1)) / 2, this->_cols - row, this->_rows);
//     }

//     inline auto _column(size_t col) {
//         if constexpr (OrderT == OrderType::RowMajor)
//             return NonContiguousBlockPtrBuffer<T>(values.data() + (col * (2 * this->_rows - col + 1)) / 2, col + 1, this->_cols);
//         else if constexpr (OrderT == OrderType::ColumnMajor)
//             return FixedStridePtrBuffer<T>(values.data() + (col * (col + 1)) / 2, col + 1);
//     }

//     inline auto _column(size_t col) const {
//         if constexpr (OrderT == OrderType::RowMajor)
//             return NonContiguousBlockPtrBuffer<const T>(values.data() + (col * (2 * this->_rows - col + 1)) / 2, col + 1, this->_cols);
//         else if constexpr (OrderT == OrderType::ColumnMajor)
//             return FixedStridePtrBuffer<const T>(values.data() + (col * (col + 1)) / 2, col + 1);
//     }
// };



// // ==============================
// // General Matrix: Lower Triangular
// // ==============================
// template <typename T, OrderType OrderT>
// class DenseMatrixAccessor<T, FormType::LowerTriangular, OrderT>
//     : public MatrixDataBase<DenseMatrixAccessor<T, FormType::LowerTriangular, OrderT>, T, FormType::LowerTriangular, OrderT> {

// protected:
//     friend class MatrixDataBase<DenseMatrixAccessor<T, FormType::LowerTriangular, OrderT>, T, FormType::LowerTriangular, OrderT>;
//     inline T& _element(size_t i, size_t j) {
//         if (i < j) throw std::out_of_range("Accessing non-stored element in lower triangular matrix.");
//         return values[(j * (2 * this->_rows - j + 1)) / 2 + (i - j)];
//     }

//     inline const T& _element(size_t i, size_t j) const {
//         if (i < j) throw std::out_of_range("Accessing non-stored element in lower triangular matrix.");
//         return values[(j * (2 * this->_rows - j + 1)) / 2 + (i - j)];
//     }

//     inline auto _row(size_t row) {
//         if constexpr (OrderT == OrderType::RowMajor)
//             return FixedStridePtrBuffer<T>(values.data() + (row * (row + 1)) / 2, row + 1);
//         else if constexpr (OrderT == OrderType::ColumnMajor)
//             return NonContiguousBlockPtrBuffer<T>(values.data() + (row * (2 * this->_rows - row + 1)) / 2, this->_cols - row, this->_rows);
//     }

//     inline auto _row(size_t row) const {
//         if constexpr (OrderT == OrderType::RowMajor)
//             return FixedStridePtrBuffer<const T>(values.data() + (row * (row + 1)) / 2, row + 1);
//         else if constexpr (OrderT == OrderType::ColumnMajor)
//             return NonContiguousBlockPtrBuffer<const T>(values.data() + (row * (2 * this->_rows - row + 1)) / 2, this->_cols - row, this->_rows);
//     }

//     inline auto _column(size_t col) {
//         if constexpr (OrderT == OrderType::RowMajor)
//             return NonContiguousBlockPtrBuffer<T>(values.data() + (col * (2 * this->_rows - col + 1)) / 2, this->_rows - col, this->_cols);
//         else if constexpr (OrderT == OrderType::ColumnMajor)
//             return FixedStridePtrBuffer<T>(values.data() + (col * (col + 1)) / 2, col + 1);
//     }

//     inline auto _column(size_t col) const {
//         if constexpr (OrderT == OrderType::RowMajor)
//             return NonContiguousBlockPtrBuffer<const T>(values.data() + (col * (2 * this->_rows - col + 1)) / 2, this->_rows - col, this->_cols);
//         else if constexpr (OrderT == OrderType::ColumnMajor)
//             return FixedStridePtrBuffer<const T>(values.data() + (col * (col + 1)) / 2, col + 1);
//     }
// };




#endif // DENSEMATRIXACCESSOR_TRIANGULARTSPECIALIZATION_H