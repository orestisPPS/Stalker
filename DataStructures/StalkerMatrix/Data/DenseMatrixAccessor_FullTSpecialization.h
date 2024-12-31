#ifndef DENSEMATRIXDATAACCESSOR_TSPECIALIZATION_H
#define DENSEMATRIXDATAACCESSOR_TSPECIALIZATION_H
#include "MatrixAccessorBase.h"

//=================================================================================================
// ======================== Fuil Dense Row Major Matrix Template Specialization ===================
//=================================================================================================

template <typename T, FormType FormT, OrderType OrderT>
class DenseMatrixAccessor : public MatrixAccessorBase<DenseMatrixAccessor<T, FormT, OrderT>, T, FormT, OrderT> {

using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormT, OrderT>, T, FormT, OrderT>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize) {
            checkSize(numValues, rows * cols, "DenseMatrixAccessor:: Constructor");  
        }
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend class Base;
    std::vector<T> values; ///< Flat array for symmetric matrix data

    inline T& _element(size_t i, size_t j) {
        return values[i * this->_cols + j];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[i * this->_cols + j];
    }
    inline auto _row(size_t row) {
        return GeneralMatrixPtrBuffer<T>(values.data() + row * this->_cols, this->_cols);
    }
    inline auto _row(size_t row) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + row * this->_cols, this->_cols);
    }
    inline auto _column(size_t col) {
        return GeneralMatrixPtrBuffer<T>(values.data() + col, this->_rows, this->_rows);
    }
    inline auto _column(size_t col) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + col, this->_rows, this->_cols);
    }
};

//=================================================================================================
// ======================== Fuil Dense Col Major Matrix Template Specialization ===================
//=================================================================================================
template <typename T, FormType FormT>
class DenseMatrixAccessor : public MatrixAccessorBase<DenseMatrixAccessor<T, FormT, OrderType::ColumnMajor>, T, FormT, OrderType::ColumnMajor> {

using Base = MatrixAccessorBase<DenseMatrixAccessor<T, FormT, OrderType::ColumnMajor>, T, FormT, OrderType::ColumnMajor>;

public:

    DenseMatrixAccessor(size_t rows, size_t cols) : Base(rows, cols), values(rows * (rows + 1) / 2) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T& value) : Base(rows, cols), values(rows * (rows + 1) / 2, value) {}
    DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues) : Base(rows, cols) {
        if constexpr (RawDoggySize)
            checkSize(numValues, rows * cols, "DenseMatrixAccessor:: Constructor");  
        this->values = std::vector<T>(values, values + numValues);
    }

protected:

    friend class Base;
    std::vector<T> values;

    inline T& _element(size_t i, size_t j) {
        return values[j * this->_rows + i];
    }
    inline const T& _element(size_t i, size_t j) const {
        return values[j * this->_rows + i];
    }
    inline auto _row(size_t row) {
        return GeneralMatrixPtrBuffer<T>(values.data() + row, this->_cols, this->_cols);
    }
    inline auto _row(size_t row) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + row, this->_cols, this->_cols);
    }
    inline auto _column(size_t col) {
        return GeneralMatrixPtrBuffer<T>(values.data() + col * this->_rows, this->_rows);
    }
    inline auto _column(size_t col) const {
        return GeneralMatrixPtrBuffer<const T>(values.data() + col * this->_rows, this->_rows);
    }
};

// // ==============================
// // General Matrix: Upper Triangular
// // ==============================
// template <typename T, OrderType OrderT>
// class DenseMatrixAccessor<T, FormType::UpperTriangular, OrderT>
//     : public MatrixDataBase<DenseMatrixAccessor<T, FormType::UpperTriangular, OrderT>, T, FormType::UpperTriangular, OrderT> {
// public:
//     DenseMatrixAccessor(size_t rows, size_t cols)
//         : MatrixDataBase<DenseMatrixAccessor, T, FormType::UpperTriangular, OrderT>(rows, cols),
//           values(rows * (rows + 1) / 2) {}

//     DenseMatrixAccessor(size_t rows, size_t cols, const T& value)
//         : MatrixDataBase<DenseMatrixAccessor, T, FormType::UpperTriangular, OrderT>(rows, cols),
//           values(rows * (rows + 1) / 2, value) {}

//     DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues)
//         : MatrixDataBase<DenseMatrixAccessor, T, FormType::UpperTriangular, OrderT>(rows, cols) {
//             if (numValues != rows * (rows + 1) / 2)
//                 throw std::invalid_argument("Invalid number of values for upper triangular matrix.");
//             this->values = std::vector<T>(values, values + numValues);
//         }

//     std::vector<T> values; ///< Flat array for upper triangular matrix data

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
// public:
//     DenseMatrixAccessor(size_t rows, size_t cols)
//         : MatrixDataBase<DenseMatrixAccessor, T, FormType::LowerTriangular, OrderT>(rows, cols),
//           values(rows * (rows + 1) / 2) {}

//     DenseMatrixAccessor(size_t rows, size_t cols, const T& value)
//         : MatrixDataBase<DenseMatrixAccessor, T, FormType::LowerTriangular, OrderT>(rows, cols),
//           values(rows * (rows + 1) / 2, value) {}

//     DenseMatrixAccessor(size_t rows, size_t cols, const T* values, size_t numValues)
//         : MatrixDataBase<DenseMatrixAccessor, T, FormType::LowerTriangular, OrderT>(rows, cols){
//             if (numValues != rows * (rows + 1) / 2)
//                 throw std::invalid_argument("Invalid number of values for lower triangular matrix.");
//                 this->values = std::vector<T>(values, values + numValues);
//         }


//     std::vector<T> values; ///< Flat array for lower triangular matrix data

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

#endif // DENSEMATRIXDATAACCESSOR_TSPECIALIZATION_H