/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "../STLKR_TestBase.h"
#include "../StalkerMatrix/StalkerMatrix.h"

namespace STLKR_Tests {

    template <typename T>
    class DataAccessTests : public STLKR_TestBase {
    public:
        explicit DataAccessTests()
            : STLKR_TestBase("Stalker Matrix Data Access Tests"),
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
            _testAccessor("Dense Full Row Major", _nonSymmetricRowMajor, _nonSymmetricRowMajorValues, _nonSymmetricExpectedRows, _nonSymmetricExpectedColumns);
            _testAccessor("Dense Full Column Major", _nonSymmetricColumnMajor, _nonSymmetricColumnMajorValues, _nonSymmetricExpectedRows, _nonSymmetricExpectedColumns);
            _testAccessor("Dense Symmetric Row Major", _symmetricRowMajor, _symmetricRowMajorValues, _symmetricExpectedRows, _symmetricExpectedColumns);
            _testAccessor("Dense Symmetric Column Major", _symmetricColumnMajor, _symmetricColumnMajorValues, _symmetricExpectedRows, _symmetricExpectedColumns);
            _testAccessor("Dense Upper Triangular Row Major", _upperTriangularRowMajor, _symmetricRowMajorValues, _symmetricExpectedRows, _symmetricExpectedColumns);
            _testAccessor("Dense Upper Triangular Column Major", _upperTriangularColumnMajor, _symmetricRowMajorValues, _symmetricExpectedRows, _symmetricExpectedColumns);
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

        StalkerMatrix<T, StorageLayout::Dense, T_Form::Full,            OrderType::RowMajor   > _nonSymmetricRowMajor;
        StalkerMatrix<T, StorageLayout::Dense, T_Form::Full,            OrderType::ColumnMajor> _nonSymmetricColumnMajor;
        StalkerMatrix<T, StorageLayout::Dense, T_Form::Symmetric,       OrderType::RowMajor   > _symmetricRowMajor;
        StalkerMatrix<T, StorageLayout::Dense, T_Form::Symmetric,       OrderType::ColumnMajor> _symmetricColumnMajor;
        StalkerMatrix<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::RowMajor   > _upperTriangularRowMajor;
        StalkerMatrix<T, StorageLayout::Dense, T_Form::UpperTriangular, OrderType::ColumnMajor> _upperTriangularColumnMajor;
        StalkerMatrix<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::RowMajor   > _lowerTriangularRowMajor;
        StalkerMatrix<T, StorageLayout::Dense, T_Form::LowerTriangular, OrderType::ColumnMajor> _lowerTriangularColumnMajor;

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


        template <T_Form form, OrderType order>
        void _testAccessor(std::string name, StalkerMatrix<T, StorageLayout::Dense, form, order>& matrix, const std::vector<T>& values,
                           const std::vector<std::vector<T>>& expectedRows, const std::vector<std::vector<T>>& expectedColumns) {

            Printers::printTitle(name, "-", T_Color::WHITE);

            Printers::printSubtitle("Size", T_Color::PATSIOURA_RED);
            {
                // Rows size
                std::vector<size_t> computedRowSizes(matrix.getNumRows());
                std::vector<size_t> expectedRowSizes(expectedRows.size());
                for (size_t i = 0; i < matrix.getNumRows(); ++i) {
                    computedRowSizes[i] = matrix.getRow(i).size();
                    expectedRowSizes[i] = expectedRows[i].size();
                }
                TestUtility::printComparisonTable(computedRowSizes, expectedRowSizes, "Rows", false);

                // Columns size
                std::vector<size_t> computedColSizes(matrix.getNumColumns());
                std::vector<size_t> expectedColSizes(expectedColumns.size());
                for (size_t i = 0; i < matrix.getNumColumns(); ++i) {
                    computedColSizes[i] = matrix.getColumn(i).size();
                    expectedColSizes[i] = expectedColumns[i].size();
                }
                TestUtility::printComparisonTable(computedColSizes, expectedColSizes, "Columns", false);
            }

            Printers::printSubtitle("Iterator Based Values", T_Color::PATSIOURA_RED);
            {
                // Rows (iterator)
                for (size_t i = 0; i < matrix.getNumRows(); ++i) {
                    auto row = matrix.getRow(i);
                    TestUtility::printComparisonTable(std::vector<T>(row.begin(), row.end()), expectedRows[i], "Row " + std::to_string(i), false);
                }
                // Columns (iterator)
                for (size_t i = 0; i < matrix.getNumColumns(); ++i) {
                    auto column = matrix.getColumn(i);
                    TestUtility::printComparisonTable(std::vector<T>(column.begin(), column.end()), expectedColumns[i], "Column " + std::to_string(i), false);
                }
            }

            Printers::printSubtitle("Const Iterator Based Values", T_Color::PATSIOURA_RED);
            {
                // Rows (const iterator)
                for (size_t i = 0; i < matrix.getNumRows(); ++i) {
                    auto row = matrix.getRow(i);
                    TestUtility::printComparisonTable(std::vector<T>(row.cbegin(), row.cend()), expectedRows[i], "Row " + std::to_string(i), false);
                }
                // Columns (const iterator)
                for (size_t i = 0; i < matrix.getNumColumns(); ++i) {
                    auto column = matrix.getColumn(i);
                    TestUtility::printComparisonTable(std::vector<T>(column.cbegin(), column.cend()), expectedColumns[i], "Column " + std::to_string(i), false);
                }
            }

            Printers::printSubtitle("Index Based Values", T_Color::PATSIOURA_RED);
            {
                // Rows (index-based)
                for (size_t i = 0; i < matrix.getNumRows(); ++i) {
                    auto row = matrix.getRow(i);
                    auto indexValues = std::vector<T>(row.size());
                    for (size_t j = 0; j < row.size(); ++j)
                        indexValues[j] = row[j];
                    TestUtility::printComparisonTable(indexValues, expectedRows[i], "Row " + std::to_string(i), false);
                }
                // Columns (index-based)
                for (size_t i = 0; i < matrix.getNumColumns(); ++i) {
                    auto column = matrix.getColumn(i);
                    auto indexValues = std::vector<T>(column.size());
                    for (size_t j = 0; j < column.size(); ++j)
                        indexValues[j] = column[j];
                    TestUtility::printComparisonTable(indexValues, expectedColumns[i], "Column " + std::to_string(i), false);
                }
            }
        }
    };
} // namespace STLKR_Tests