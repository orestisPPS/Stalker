#ifndef MATRIXDATA_VALIDITYTESTS_H
#define MATRIXDATA_VALIDITYTESTS_H

#include "../STLKR_TestBase.h"
#include "../StalkerMatrix/Data/DenseMatrixAccessor_FullTSpecialization.h"
#include "../StalkerMatrix/Data/DenseMatrixAccessor_SymmetricTSpecialization.h"
#include "../StalkerMatrix/Data/DenseMatrixAccessor_TriangularTSpecialization.h"
// #include "../StalkerMatrix/Data/DenseMatrixAccessor_UpperTriangularTSpecialization.h"
#include "../TestUtility.h"
#include <iostream>
#include <vector>
#include <map>

namespace STLKR_Tests {

    template <typename T>
    class DataAccess_ValidityTests : public STLKR_TestBase {
    public:
        explicit DataAccess_ValidityTests()
            : STLKR_TestBase("Matrix Data Accessor Tests"),
              _numRows(4), _numCols(4),
              _nonSymmetricRowMajorValues{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              _nonSymmetricColumnMajorValues{0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15},
              _symmetricRowMajorValues{0, 1, 2, 3, 5, 6, 7, 10, 11, 15},
              _symmetricColumnMajorValues{0, 1, 5, 2, 6, 10, 3, 7, 11, 15},
              _lowerTriangularRowMajorValues{0, 4, 5, 8, 9, 10, 12, 13, 14, 15},
              _lowerTriangularColumnMajorValues{0, 4, 8, 12, 5, 9, 13, 10, 14, 15},
              _nonSymmetricRowMajor(_numRows, _numCols, _nonSymmetricRowMajorValues.data(), _nonSymmetricRowMajorValues.size()),
              _nonSymmetricColumnMajor(_numRows, _numCols, _nonSymmetricColumnMajorValues.data(), _nonSymmetricColumnMajorValues.size()),
              _symmetricRowMajor(_numRows, _numCols, _symmetricRowMajorValues.data(), _symmetricRowMajorValues.size()),
              _symmetricColumnMajor(_numRows, _numCols, _symmetricColumnMajorValues.data(), _symmetricColumnMajorValues.size()),
              _upperTriangularRowMajor(_numRows, _numCols, _symmetricRowMajorValues.data(), _symmetricRowMajorValues.size()),
              _upperTriangularColumnMajor(_numRows, _numCols, _symmetricColumnMajorValues.data(), _symmetricColumnMajorValues.size()),
              _lowerTriangularRowMajor(_numRows, _numCols, _lowerTriangularRowMajorValues.data(), _lowerTriangularRowMajorValues.size()),
              _lowerTriangularColumnMajor(_numRows, _numCols, _lowerTriangularColumnMajorValues.data(), _lowerTriangularColumnMajorValues.size()) {}

        void runTest() override {
            Printers::printTitle(this->_testName, "=", ColourType::BRIGHT_WHITE);
            // _testAccessor("Dense Full Row Major", _nonSymmetricRowMajor, _nonSymmetricRowMajorValues, _nonSymmetricExpectedRows, _nonSymmetricExpectedColumns);
            // _testAccessor("Dense Full Column Major", _nonSymmetricColumnMajor, _nonSymmetricColumnMajorValues, _nonSymmetricExpectedRows, _nonSymmetricExpectedColumns);
            // _testAccessor("Dense Symmetric Row Major", _symmetricRowMajor, _symmetricRowMajorValues, _symmetricExpectedRows, _symmetricExpectedColumns);
            // _testAccessor("Dense Symmetric Column Major", _symmetricColumnMajor, _symmetricColumnMajorValues, _symmetricExpectedRows, _symmetricExpectedColumns);
            // _testAccessor("Dense Upper Triangular Row Major", _upperTriangularRowMajor, _symmetricRowMajorValues, _symmetricExpectedRows, _symmetricExpectedColumns);
            // _testAccessor("Dense Upper Triangular Column Major", _upperTriangularColumnMajor, _symmetricRowMajorValues, _symmetricExpectedRows, _symmetricExpectedColumns);
            _testAccessor("Dense Lower Triangular Row Major", _lowerTriangularRowMajor, _lowerTriangularRowMajorValues, _lowerTriangularExpectedRows, _lowerTriangularExpectedColumns);
            _testAccessor("Dense Lower Triangular Column Major", _lowerTriangularColumnMajor, _lowerTriangularColumnMajorValues, _lowerTriangularExpectedRows, _lowerTriangularExpectedColumns);
        }

    private:
        int _numRows;
        int _numCols;

        // Value vectors
        std::vector<T> _nonSymmetricRowMajorValues;
        std::vector<T> _nonSymmetricColumnMajorValues;
        std::vector<T> _symmetricRowMajorValues;
        std::vector<T> _symmetricColumnMajorValues;
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


        template <FormType form, OrderType order>
        void _testAccessor(std::string name, DenseMatrixAccessor<T, form, order>& accessor, const std::vector<T>& values,
                           const std::vector<std::vector<T>>& expectedRows, const std::vector<std::vector<T>>& expectedColumns) {

            Printers::printTitle(name, "*", ColourType::GANDALF_GRAY);

            std::vector<size_t> computedRowSizes(accessor.getNumRows());
            std::vector<size_t> expectedRowSizes(expectedRows.size());
            for (size_t i = 0; i < accessor.getNumRows(); ++i){
                computedRowSizes[i] = accessor.row(i).size();
                expectedRowSizes[i] = expectedRows[i].size();
            }
            TestUtility::printComparisonTable(computedRowSizes, expectedRowSizes, "Rows Sizes", false);

            std::vector<size_t> computedColSizes(accessor.getNumCols());
            std::vector<size_t> expectedColSizes(expectedRows.size());
            for (size_t i = 0; i < accessor.getNumCols(); ++i){
                computedColSizes[i] = accessor.column(i).size();
                expectedColSizes[i] = expectedColumns[i].size();
            }
            TestUtility::printComparisonTable(computedRowSizes, expectedRowSizes, "Columns Sizes", false);

            for (size_t i = 0; i < accessor.getNumRows(); ++i) {
                auto row = accessor.row(i);
                TestUtility::printComparisonTable(std::vector<T>(row.begin(), row.end()), expectedRows[i], "Row " + std::to_string(i) + " Values", false);
            }

            for (size_t i = 0; i < accessor.getNumCols(); ++i) {
                auto column = accessor.column(i);
                TestUtility::printComparisonTable(std::vector<T>(column.begin(), column.end()), expectedColumns[i], "Column " + std::to_string(i) + " Values", false);
            }
        }
    };
} // namespace STLKR_Tests

#endif // MATRIXDATA_VALIDITYTESTS_H
