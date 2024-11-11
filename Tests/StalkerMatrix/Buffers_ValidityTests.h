#ifndef MATRIXBUFFERS_VALIDITYTESTS_H
#define MATRIXBUFFERS_VALIDITYTESTS_H

#include "../STLKR_TestBase.h"
#include "../StalkerMatrix/MatrixBuffers.h"
#include <vector>
#include <iostream>

namespace STLKR_Tests {

    class MatrixBuffers_ValidityTests : public STLKR_TestBase {
    public:
        explicit MatrixBuffers_ValidityTests()
            : STLKR_TestBase("Matrix Buffer Tests") {}

        void runTest() override {
            std::cout << "Running Matrix Buffer Tests..." << std::endl;
            _testRowBuffer();
            _testColumnBuffer();
        }

    private:
        void _testRowBuffer() {
            std::cout << "Testing MatrixRowBuffer..." << std::endl;

            // Define a 4x5 matrix (4 rows, 5 columns)
            std::vector<int> matrixData = _generateMatrixData(4, 5);

            // Test Row Major Row Buffer
            MatrixRowBuffer<int, MatrixElementsOrder::RowMajor> rowBufferRowMajor(matrixData.data(), 5, 4);
            printTestCaseResult(_testRowElementAccess<MatrixRowBuffer<int, MatrixElementsOrder::RowMajor>>(rowBufferRowMajor, 4, 5, MatrixElementsOrder::RowMajor), "Row Major - Row Element Access");
            printTestCaseResult(_testRowIterator<MatrixRowBuffer<int, MatrixElementsOrder::RowMajor>>(rowBufferRowMajor, 4, 5, MatrixElementsOrder::RowMajor), "Row Major - Row Iterators");
            printTestCaseResult(_testRowConstIterator<MatrixRowBuffer<int, MatrixElementsOrder::RowMajor>>(rowBufferRowMajor, 4, 5, MatrixElementsOrder::RowMajor), "Row Major - Row Const Iterators");

            // Test Column Major Row Buffer
            MatrixRowBuffer<int, MatrixElementsOrder::ColumnMajor> rowBufferColumnMajor(matrixData.data(), 5, 4);
            printTestCaseResult(_testRowElementAccess<MatrixRowBuffer<int, MatrixElementsOrder::ColumnMajor>>(rowBufferColumnMajor, 4, 5, MatrixElementsOrder::ColumnMajor), "Column Major - Row Element Access");
            printTestCaseResult(_testRowIterator<MatrixRowBuffer<int, MatrixElementsOrder::ColumnMajor>>(rowBufferColumnMajor, 4, 5, MatrixElementsOrder::ColumnMajor), "Column Major - Row Iterators");
            printTestCaseResult(_testRowConstIterator<MatrixRowBuffer<int, MatrixElementsOrder::ColumnMajor>>(rowBufferColumnMajor, 4, 5, MatrixElementsOrder::ColumnMajor), "Column Major - Row Const Iterators");
        }

        void _testColumnBuffer() {
            std::cout << "Testing MatrixColumnBuffer..." << std::endl;

            // Define a 4x5 matrix (4 rows, 5 columns)
            std::vector<int> matrixData = _generateMatrixData(4, 5);

            // Test Row Major Column Buffer
            MatrixColumnBuffer<int, MatrixElementsOrder::RowMajor> columnBufferRowMajor(matrixData.data(), 4, 5);
            printTestCaseResult(_testColumnElementAccess<MatrixColumnBuffer<int, MatrixElementsOrder::RowMajor>>(columnBufferRowMajor, 4, 5, MatrixElementsOrder::RowMajor), "Row Major - Column Element Access");
            printTestCaseResult(_testColumnIterator<MatrixColumnBuffer<int, MatrixElementsOrder::RowMajor>>(columnBufferRowMajor, 4, 5, MatrixElementsOrder::RowMajor), "Row Major - Column Iterators");
            printTestCaseResult(_testColumnConstIterator<MatrixColumnBuffer<int, MatrixElementsOrder::RowMajor>>(columnBufferRowMajor, 4, 5, MatrixElementsOrder::RowMajor), "Row Major - Column Const Iterators");

            // Test Column Major Column Buffer
            MatrixColumnBuffer<int, MatrixElementsOrder::ColumnMajor> columnBufferColumnMajor(matrixData.data(), 4, 5);
            printTestCaseResult(_testColumnElementAccess<MatrixColumnBuffer<int, MatrixElementsOrder::ColumnMajor>>(columnBufferColumnMajor, 4, 5, MatrixElementsOrder::ColumnMajor), "Column Major - Column Element Access");
            printTestCaseResult(_testColumnIterator<MatrixColumnBuffer<int, MatrixElementsOrder::ColumnMajor>>(columnBufferColumnMajor, 4, 5, MatrixElementsOrder::ColumnMajor), "Column Major - Column Iterators");
            printTestCaseResult(_testColumnConstIterator<MatrixColumnBuffer<int, MatrixElementsOrder::ColumnMajor>>(columnBufferColumnMajor, 4, 5, MatrixElementsOrder::ColumnMajor), "Column Major - Column Const Iterators");
        }

        std::vector<int> _generateMatrixData(size_t rows, size_t cols) {
            std::vector<int> data(rows * cols);
            for (size_t i = 0; i < rows * cols; ++i) {
                data[i] = static_cast<int>(i);
            }
            return data;
        }

        template <typename BufferType>
        bool _testRowElementAccess(BufferType& rowBuffer, size_t rows, size_t cols, MatrixElementsOrder orderType) {
            for (std::size_t i = 0; i < rowBuffer.size(); ++i) {
                int expectedValue = (orderType == MatrixElementsOrder::RowMajor)
                                    ? static_cast<int>(i)
                                    : static_cast<int>(i * rows);
                if (rowBuffer[i] != expectedValue) {
                    std::cout << "Expected: " << expectedValue << ", Found: " << rowBuffer[i] << " at index " << i << std::endl;
                    return false;
                }
            }
            return true;
        }

        template <typename BufferType>
        bool _testRowIterator(BufferType& rowBuffer, size_t rows, size_t cols, MatrixElementsOrder orderType) {
            std::size_t i = 0;
            for (auto it = rowBuffer.begin(); it != rowBuffer.end(); ++it, ++i) {
                int expectedValue = (orderType == MatrixElementsOrder::RowMajor)
                                    ? static_cast<int>(i)
                                    : static_cast<int>(i * rows);
                if (*it != expectedValue) {
                    std::cout << "Expected: " << expectedValue << ", Found: " << *it << " at iterator position " << i << std::endl;
                    return false;
                }
            }
            return true;
        }

        template <typename BufferType>
        bool _testRowConstIterator(const BufferType& rowBuffer, size_t rows, size_t cols, MatrixElementsOrder orderType) {
            std::size_t i = 0;
            for (auto it = rowBuffer.cbegin(); it != rowBuffer.cend(); ++it, ++i) {
                int expectedValue = (orderType == MatrixElementsOrder::RowMajor)
                                    ? static_cast<int>(i)
                                    : static_cast<int>(i * rows);
                if (*it != expectedValue) {
                    std::cout << "Expected: " << expectedValue << ", Found: " << *it << " at const iterator position " << i << std::endl;
                    return false;
                }
            }
            return true;
        }

        template <typename BufferType>
        bool _testColumnElementAccess(BufferType& columnBuffer, size_t rows, size_t cols, MatrixElementsOrder orderType) {
            for (std::size_t i = 0; i < columnBuffer.size(); ++i) {
                int expectedValue = (orderType == MatrixElementsOrder::RowMajor)
                                    ? static_cast<int>(i * cols)
                                    : static_cast<int>(i);
                if (columnBuffer[i] != expectedValue) {
                    std::cout << "Expected: " << expectedValue << ", Found: " << columnBuffer[i] << " at index " << i << std::endl;
                    return false;
                }
            }
            return true;
        }

        template <typename BufferType>
        bool _testColumnIterator(BufferType& columnBuffer, size_t rows, size_t cols, MatrixElementsOrder orderType) {
            std::size_t i = 0;
            for (auto it = columnBuffer.begin(); it != columnBuffer.end(); ++it, ++i) {
                int expectedValue = (orderType == MatrixElementsOrder::RowMajor)
                                    ? static_cast<int>(i * cols)
                                    : static_cast<int>(i);
                if (*it != expectedValue) {
                    std::cout << "Expected: " << expectedValue << ", Found: " << *it << " at iterator position " << i << std::endl;
                    return false;
                }
            }
            return true;
        }

        template <typename BufferType>
        bool _testColumnConstIterator(const BufferType& columnBuffer, size_t rows, size_t cols, MatrixElementsOrder orderType) {
            std::size_t i = 0;
            for (auto it = columnBuffer.cbegin(); it != columnBuffer.cend(); ++it, ++i) {
                int expectedValue = (orderType == MatrixElementsOrder::RowMajor)
                                    ? static_cast<int>(i * cols)
                                    : static_cast<int>(i);
                if (*it != expectedValue) {
                    std::cout << "Expected: " << expectedValue << ", Found: " << *it << " at const iterator position " << i << std::endl;
                    return false;
                }
            }
            return true;
        }
    };

} // namespace STLKR_Tests

#endif // MATRIXBUFFERS_VALIDITYTESTS_H
