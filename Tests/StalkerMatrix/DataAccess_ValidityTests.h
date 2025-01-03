#ifndef MATRIXDATA_VALIDITYTESTS_H
#define MATRIXDATA_VALIDITYTESTS_H

#include "../STLKR_TestBase.h"
#include "../StalkerMatrix/Data/DenseMatrixAccessor_FullTSpecialization.h"
#include "../StalkerMatrix/Data/DenseMatrixAccessor_SymmetricTSpecialization.h"
// #include "../StalkerMatrix/Data/DenseMatrixAccessor_UpperTriangularTSpecialization.h"
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
              _nonSymmetricRowMajorValues{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              _nonSymmetricColumnMajorValues{0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15},
              _symmetricRowMajorValues{0, 1, 2, 3, 5, 6, 7, 10, 11, 15},
              _symmetricColumnMajorValues{0, 1, 5, 2, 6, 10, 3, 7, 11, 15},
              _upperTriangularRowMajorValues{0, 1, 2, 3, 5, 6, 7, 10, 11, 15},
              _upperTriangularColumnMajorValues{0, 1, 5, 2, 6, 10, 3, 7, 11, 15},
              _lowerTriangularRowMajorValues{0, 4, 5, 8, 9, 10, 12, 13, 14, 15},
              _lowerTriangularColumnMajorValues{0, 4, 8, 12, 5, 9, 13, 10, 14, 15},
              _nonSymmetricRowMajor(_numRows, _numCols, _nonSymmetricRowMajorValues.data(), _nonSymmetricRowMajorValues.size()),
              _nonSymmetricColumnMajor(_numRows, _numCols, _nonSymmetricColumnMajorValues.data(), _nonSymmetricColumnMajorValues.size()),
              _symmetricRowMajor(_numRows, _numCols, _symmetricRowMajorValues.data(), _symmetricRowMajorValues.size()),
              _symmetricColumnMajor(_numRows, _numCols, _symmetricColumnMajorValues.data(), _symmetricColumnMajorValues.size()),
              _upperTriangularRowMajor(_numRows, _numCols, _upperTriangularRowMajorValues.data(), _upperTriangularRowMajorValues.size()),
              _upperTriangularColumnMajor(_numRows, _numCols, _upperTriangularColumnMajorValues.data(), _upperTriangularColumnMajorValues.size()),
              _lowerTriangularRowMajor(_numRows, _numCols, _lowerTriangularRowMajorValues.data(), _lowerTriangularRowMajorValues.size()),
              _lowerTriangularColumnMajor(_numRows, _numCols, _lowerTriangularColumnMajorValues.data(), _lowerTriangularColumnMajorValues.size()) {}

        void runTest() override {
            std::cout << "Running Matrix Data Tests..." << std::endl;
            _testNonSymmetricColumnMajor();
            _testNonSymmetricRowMajor();
            _testSymmetricRowMajor();
            _testSymmetricColumnMajor();

        }

    private:
        int _numRows;
        int _numCols;

        // Value vectors
        std::vector<T> _nonSymmetricRowMajorValues;
        std::vector<T> _nonSymmetricColumnMajorValues;
        std::vector<T> _symmetricRowMajorValues;
        std::vector<T> _symmetricColumnMajorValues;
        std::vector<T> _upperTriangularRowMajorValues;
        std::vector<T> _upperTriangularColumnMajorValues;
        std::vector<T> _lowerTriangularRowMajorValues;
        std::vector<T> _lowerTriangularColumnMajorValues;

        // Matrix data
        DenseMatrixAccessor<T, FormType::Full,            OrderType::RowMajor   > _nonSymmetricRowMajor;
        DenseMatrixAccessor<T, FormType::Full,            OrderType::ColumnMajor> _nonSymmetricColumnMajor;
        DenseMatrixAccessor<T, FormType::Symmetric,       OrderType::RowMajor   > _symmetricRowMajor;
        DenseMatrixAccessor<T, FormType::Symmetric,       OrderType::ColumnMajor> _symmetricColumnMajor;
        DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::RowMajor   > _upperTriangularRowMajor;
        DenseMatrixAccessor<T, FormType::UpperTriangular, OrderType::ColumnMajor> _upperTriangularColumnMajor;
        DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::RowMajor   > _lowerTriangularRowMajor;
        DenseMatrixAccessor<T, FormType::LowerTriangular, OrderType::ColumnMajor> _lowerTriangularColumnMajor;

        std::vector<std::vector<T>> _nonSymmetricExpectedRows = {
            {0, 1, 2, 3},
            {4, 5, 6, 7},
            {8, 9, 10, 11},
            {12, 13, 14, 15}
        };

        std::vector<std::vector<T>> _nonSymmetricExpectedColumns = {
            {0, 4, 8, 12},
            {1, 5, 9, 13},
            {2, 6, 10, 14},
            {3, 7, 11, 15}
        };

        void _testNonSymmetricRowMajor() {
            for (int i = 0; i < _numRows; ++i) {
                auto row = _nonSymmetricRowMajor.row(i);
                printTestCaseResult(row.size() == _numCols, "Non-Symmetric Row Major Row " + std::to_string(i) + " Buffer Size");
                printTestCaseResult(std::vector<T>(row.begin(), row.end()) == _nonSymmetricExpectedRows[i], "Non-Symmetric Row Major Row " + std::to_string(i) + " Buffer Values");
            }
            for (int i = 0; i < _numCols; ++i) {
                auto column = _nonSymmetricRowMajor.column(i);
                printTestCaseResult(column.size() == _numRows, "Non-Symmetric Row Major Column " + std::to_string(i) + " Buffer Size");
                printTestCaseResult(std::vector<T>(column.begin(), column.end()) == _nonSymmetricExpectedColumns[i], "Non-Symmetric Row Major Column " + std::to_string(i) + " Buffer Values");
            }
        }

        void _testNonSymmetricColumnMajor() {
            for (int i = 0; i < _numRows; ++i) {
                auto row = _nonSymmetricColumnMajor.row(i);
                printTestCaseResult(row.size() == _numCols, "Non-Symmetric Column Major Row " + std::to_string(i) + " Buffer Size");
                printTestCaseResult(std::vector<T>(row.begin(), row.end()) == _nonSymmetricExpectedRows[i], "Non-Symmetric Column Major Row " + std::to_string(i) + " Buffer Values");
            }
            for (int i = 0; i < _numCols; ++i) {
                auto column = _nonSymmetricColumnMajor.column(i);
                printTestCaseResult(column.size() == _numRows, "Non-Symmetric Column Major Column " + std::to_string(i) + " Buffer Size");
                printTestCaseResult(std::vector<T>(column.begin(), column.end()) == _nonSymmetricExpectedColumns[i], "Non-Symmetric Column Major Column " + std::to_string(i) + " Buffer Values");
            }
        }

        void _testSymmetricRowMajor() {
            for (int i = 0; i < _numRows; ++i) {
                auto row = _symmetricRowMajor.row(i);
                printTestCaseResult(row.size() == _symmetricExpectedRows[i].size(), "Symmetric Row Major Row " + std::to_string(i) + " Buffer Size");
                printTestCaseResult(std::vector<T>(row.begin(), row.end()) == _symmetricExpectedRows[i], "Symmetric Row Major Row " + std::to_string(i) + " Buffer Values");
            }
            for (int i = 0; i < _numCols; ++i) {
                auto column = _symmetricRowMajor.column(i);
                printTestCaseResult(column.size() == i + 1, "Symmetric Row Major Column " + std::to_string(i) + " Buffer Size");
                auto v = std::vector<T>(column.begin(), column.end());
                bool lol;
                printTestCaseResult(std::vector<T>(column.begin(), column.end()) == _symmetricExpectedColumns[i], "Symmetric Row Major Column " + std::to_string(i) + " Buffer Values");
            }
        }

        void _testSymmetricColumnMajor() {
            for (int i = 0; i < _numRows; ++i) {
                auto row = _symmetricColumnMajor.row(i);
                printTestCaseResult(row.size() == _symmetricExpectedRows[i].size(), "Symmetric Column Major Row " + std::to_string(i) + " Buffer Size");
                printTestCaseResult(std::vector<T>(row.begin(), row.end()) == _symmetricExpectedRows[i], "Symmetric Column Major Row " + std::to_string(i) + " Buffer Values");
            }
            for (int i = 0; i < _numCols; ++i) {
                auto column = _symmetricColumnMajor.column(i);
                printTestCaseResult(column.size() == _symmetricExpectedColumns[i].size(), "Symmetric Column Major Column " + std::to_string(i) + " Buffer Size");
                printTestCaseResult(std::vector<T>(column.begin(), column.end()) == _symmetricExpectedColumns[i], "Symmetric Column Major Column " + std::to_string(i) + " Buffer Values");
                auto v = std::vector<T>(column.begin(), column.end());
                bool lol = false;
            }
        }

        // void _testSy


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
            {4, 5},
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
    };
} // namespace STLKR_Tests

#endif // MATRIXDATA_VALIDITYTESTS_H
