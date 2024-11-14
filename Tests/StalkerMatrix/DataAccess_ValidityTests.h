#ifndef MATRIXDATA_VALIDITYTESTS_H
#define MATRIXDATA_VALIDITYTESTS_H

#include "../STLKR_TestBase.h"
#include "../StalkerMatrix/Data/MatrixData.h"
#include <iostream>
#include <vector>
#include <map>

namespace STLKR_Tests {

    template <typename T>
    class DataAccess_ValidityTests : public STLKR_TestBase {
    public:
         explicit DataAccess_ValidityTests()
            : STLKR_TestBase("Matrix Data Tests"),
              _numRows(4), _numCols(4),
              _nonSymmetricRowMajor(_numRows, _numCols),
              _nonSymmetricColumnMajor(_numRows, _numCols),
              _symmetricRowMajor(_numRows, _numCols),
              _symmetricColumnMajor(_numRows, _numCols),
              _upperTriangularRowMajor(_numRows, _numCols),
              _upperTriangularColumnMajor(_numRows, _numCols),
              _lowerTriangularRowMajor(_numRows, _numCols),
              _lowerTriangularColumnMajor(_numRows, _numCols) {}

        void runTest() override {
            std::cout << "Running Matrix Data Tests..." << std::endl;
            _populate();
            _testNonSymmetric();
            _testSymmetric();
            _testUpperTriangular();
            _testLowerTriangular();
        }

    private:
        int _numRows;
        int _numCols;
        DenseMatrixData<T, FormType::NonSymmetric, OrderType::RowMajor> _nonSymmetricRowMajor;
        DenseMatrixData<T, FormType::NonSymmetric, OrderType::ColumnMajor> _nonSymmetricColumnMajor;
        DenseMatrixData<T, FormType::Symmetric, OrderType::RowMajor> _symmetricRowMajor;
        DenseMatrixData<T, FormType::Symmetric, OrderType::ColumnMajor> _symmetricColumnMajor;
        DenseMatrixData<T, FormType::UpperTriangular, OrderType::RowMajor> _upperTriangularRowMajor;
        DenseMatrixData<T, FormType::UpperTriangular, OrderType::ColumnMajor> _upperTriangularColumnMajor;
        DenseMatrixData<T, FormType::LowerTriangular, OrderType::RowMajor> _lowerTriangularRowMajor;
        DenseMatrixData<T, FormType::LowerTriangular, OrderType::ColumnMajor> _lowerTriangularColumnMajor;

        std::map<std::string, std::vector<T>> _matrixExpectedValues = {
            {"NonSymmetricRowMajor", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},
            {"NonSymmetricColumnMajor", {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15}},
            {"SymmetricRowMajor", {0, 1, 2, 3, 5, 6, 7, 10, 11, 15}},
            {"SymmetricColumnMajor", {0, 1, 5, 2, 6, 10, 3, 7, 11, 15}},
            {"UpperTriangularRowMajor", {0, 1, 2, 3, 5, 6, 7, 10, 11, 15}},
            {"UpperTriangularColumnMajor", {0, 1, 5, 2, 6, 10, 3, 7, 11, 15}},
            {"LowerTriangularRowMajor", {0, 4, 5, 8, 9, 10, 12, 13, 14, 15}},
            {"LowerTriangularColumnMajor", {0, 4, 8, 12, 5, 9, 13, 10, 14, 15}}
        };

        std::vector<std::vector<T>> _nonSymmetricExpectedRows = {
            {0, 1, 2, 3},
            {1, 5, 6, 7},
            {2, 6, 10, 11},
            {3, 7, 11, 15}
        };

        std::vector<std::vector<T>> _nonSymmetricExpectedColumns = {
            {0, 4, 8, 12},
            {1, 5, 9, 13},
            {2, 6, 10, 14},
            {3, 7, 11, 15}
        };

        std::vector<std::vector<T>> _symmetricExpectedRows = {
            {0, 1, 2, 3},
            {5, 6, 7},
            {10, 11},
            {15}
        };

        std::vector<std::vector<T>> _symmetricExpectedColumns = {
            {0},
            {1, 5},
            {2, 6, 10},
            {3, 7, 11, 15}
        };

        std::vector<std::vector<T>> _upperTriangularExpectedRows = {
            {0, 1, 2, 3},
            {5, 6, 7},
            {10, 11},
            {15}
        };

        std::vector<std::vector<T>> _upperTriangularExpectedColumns = {
            {0},
            {1, 5},
            {2, 6, 10},
            {3, 7, 11, 15}
        };

        std::vector<std::vector<T>> _lowerTriangularExpectedRows = {
            {0},
            {4, 5},
            {8, 9, 10},
            {12, 13, 14, 15}
        };

        std::vector<std::vector<T>> _lowerTriangularExpectedColumns = {
            {0, 4, 8, 12},
            {5, 9, 13},
            {10, 14},
            {15}
        };

        bool _testSymmetric(){
            bool result = true;
            for (auto i = 0; i < _numRows; i++) {
                auto iRowRowMajor = _symmetricRowMajor.row(i);
                auto iRowColumnMajor = _symmetricColumnMajor.row(i);
                auto rowVectorRowMajor = std::vector<T>(iRowRowMajor.begin(), iRowRowMajor.end());
                auto rowVectorColumnMajor = std::vector<T>(iRowColumnMajor.begin(), iRowColumnMajor.end());
                printTestCaseResult(rowVectorRowMajor == _symmetricExpectedRows[i], "Row Major Symmetric Row " + std::to_string(i));
                printTestCaseResult(rowVectorColumnMajor == _symmetricExpectedRows[i], "Column Major Symmetric Row " + std::to_string(i));
                for (auto bfr: rowVectorRowMajor) {
                    std::cout << bfr << " ";
                }
                std::cout << std::endl;
            }
            for (auto i = 0; i < _numCols; i++) {
                auto iColumnRowMajor = _symmetricRowMajor.column(i);
                auto iColumnColumnMajor = _symmetricColumnMajor.column(i);
                auto columnVectorRowMajor = std::vector<T>(iColumnRowMajor.begin(), iColumnRowMajor.end());
                auto columnVectorColumnMajor = std::vector<T>(iColumnColumnMajor.begin(), iColumnColumnMajor.end());
                printTestCaseResult(columnVectorRowMajor == _symmetricExpectedColumns[i], "Row Major Symmetric Column " + std::to_string(i));
                printTestCaseResult(columnVectorColumnMajor == _symmetricExpectedColumns[i], "Column Major Symmetric Column " + std::to_string(i));

                for (auto bfc: columnVectorRowMajor) {
                    std::cout << bfc << " ";
                }
                std::cout << std::endl;
            }
            return result;
        }

        bool _testNonSymmetric(){
            bool result = true;
            for (auto i = 0; i < _numRows; i++) {
                auto iRowRowMajor = _nonSymmetricRowMajor.row(i);
                auto iRowColumnMajor = _nonSymmetricColumnMajor.row(i);
                auto rowVectorRowMajor = std::vector<T>(iRowRowMajor.begin(), iRowRowMajor.end());
                auto rowVectorColumnMajor = std::vector<T>(iRowColumnMajor.begin(), iRowColumnMajor.end());
                printTestCaseResult(rowVectorRowMajor == _nonSymmetricExpectedRows[i], "Row MajorNon-Symmetric Row " + std::to_string(i));
                printTestCaseResult(rowVectorColumnMajor == _nonSymmetricExpectedRows[i], "Column Major Non-Symmetric Row " + std::to_string(i));
            }
            for (auto i = 0; i < _numCols; i++) {
                auto iColumnRowMajor = _nonSymmetricRowMajor.column(i);
                auto iColumnColumnMajor = _nonSymmetricColumnMajor.column(i);
                auto columnVectorRowMajor = std::vector<T>(iColumnRowMajor.begin(), iColumnRowMajor.end());
                auto columnVectorColumnMajor = std::vector<T>(iColumnColumnMajor.begin(), iColumnColumnMajor.end());
                printTestCaseResult(columnVectorRowMajor == _nonSymmetricExpectedColumns[i], "Row Major Non-Symmetric Column " + std::to_string(i));
                printTestCaseResult(columnVectorColumnMajor == _nonSymmetricExpectedColumns[i], "Column Major Non-Symmetric Column " + std::to_string(i));
            }

            return result;

        }

        bool _testUpperTriangular(){
            bool result = true;
            for (auto i = 0; i < _numRows; i++) {
                auto iRowRowMajor = _upperTriangularRowMajor.row(i);
                auto iRowColumnMajor = _upperTriangularColumnMajor.row(i);
                auto rowVectorRowMajor = std::vector<T>(iRowRowMajor.begin(), iRowRowMajor.end());
                auto rowVectorColumnMajor = std::vector<T>(iRowColumnMajor.begin(), iRowColumnMajor.end());
                printTestCaseResult(rowVectorRowMajor == _upperTriangularExpectedRows[i], "Upper Triangular Row " + std::to_string(i));
                printTestCaseResult(rowVectorColumnMajor == _upperTriangularExpectedRows[i], "Upper Triangular Row " + std::to_string(i));
            }
            for (auto i = 0; i < _numCols; i++) {
                auto iColumnRowMajor = _upperTriangularRowMajor.column(i);
                auto iColumnColumnMajor = _upperTriangularColumnMajor.column(i);
                auto columnVectorRowMajor = std::vector<T>(iColumnRowMajor.begin(), iColumnRowMajor.end());
                auto columnVectorColumnMajor = std::vector<T>(iColumnColumnMajor.begin(), iColumnColumnMajor.end());
                printTestCaseResult(columnVectorRowMajor == _upperTriangularExpectedColumns[i], "Upper Triangular Column " + std::to_string(i));
                printTestCaseResult(columnVectorColumnMajor == _upperTriangularExpectedColumns[i], "Upper Triangular Column " + std::to_string(i));
            }
            return result;
        }

        bool _testLowerTriangular(){
            bool result = true;
            for (auto i = 0; i < _numRows; i++) {
                auto iRowRowMajor = _lowerTriangularRowMajor.row(i);
                auto iRowColumnMajor = _lowerTriangularColumnMajor.row(i);
                auto rowVectorRowMajor = std::vector<T>(iRowRowMajor.begin(), iRowRowMajor.end());
                auto rowVectorColumnMajor = std::vector<T>(iRowColumnMajor.begin(), iRowColumnMajor.end());
                printTestCaseResult(rowVectorRowMajor == _lowerTriangularExpectedRows[i], "Lower Triangular Row " + std::to_string(i));
                printTestCaseResult(rowVectorColumnMajor == _lowerTriangularExpectedRows[i], "Lower Triangular Row " + std::to_string(i));
            }
            for (auto i = 0; i < _numCols; i++) {
                auto iColumnRowMajor = _lowerTriangularRowMajor.column(i);
                auto iColumnColumnMajor = _lowerTriangularColumnMajor.column(i);
                auto columnVectorRowMajor = std::vector<T>(iColumnRowMajor.begin(), iColumnRowMajor.end());
                auto columnVectorColumnMajor = std::vector<T>(iColumnColumnMajor.begin(), iColumnColumnMajor.end());
                printTestCaseResult(columnVectorRowMajor == _lowerTriangularExpectedColumns[i], "Lower Triangular Column " + std::to_string(i));
                printTestCaseResult(columnVectorColumnMajor == _lowerTriangularExpectedColumns[i], "Lower Triangular Column " + std::to_string(i));
            }
            return result;
        }




        void _populate() {
            for (size_t i = 0; i < _numRows; ++i) {
                for (size_t j = 0; j < _numCols; ++j) {
                    auto value = static_cast<T>(i * _numCols + j);
                    _nonSymmetricRowMajor.element(i, j) = value;
                    _nonSymmetricColumnMajor.element(i, j) = value;
                    if (i <= j) {
                        _upperTriangularRowMajor.element(i, j) = value;
                        _upperTriangularColumnMajor.element(i, j) = value;
                        _symmetricRowMajor.element(i, j) = value;
                        _symmetricColumnMajor.element(i, j) = value;
                    }
                    if (i >= j) {
                        _lowerTriangularRowMajor.element(i, j) = value;
                        _lowerTriangularColumnMajor.element(i, j) = value;
                    }
                }
            }
        }


    };
} // namespace STLKR_Tests

#endif // MATRIXDATA_VALIDITYTESTS_H
