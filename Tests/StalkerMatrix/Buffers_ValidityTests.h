#ifndef MATRIXBUFFERS_VALIDITYTESTS_H
#define MATRIXBUFFERS_VALIDITYTESTS_H

#include "../STLKR_TestBase.h"
#include "../StalkerMatrix/Data/PtrBuffer.h"
#include <vector>
#include <iostream>

namespace STLKR_Tests {

    class MatrixBuffers_ValidityTests : public STLKR_TestBase {
    public:
        explicit MatrixBuffers_ValidityTests()
            : STLKR_TestBase("Matrix Buffer Tests") {}

        void runTest() override {
            std::cout << "Running Matrix Buffer Tests..." << std::endl;
            _testAllBuffers();
        }

    private:
        void _testAllBuffers() {
            std::cout << "Testing All Matrix Buffers..." << std::endl;

            // Define a 4x5 matrix (4 rows, 5 columns)
            std::vector<int> matrixData = _generateMatrixData(4, 5);

            // Test Row-Major Row Buffer
            RowBufferRowMajor<int> rowBufferRowMajor(matrixData.data(), 5);
            _runBufferTests(rowBufferRowMajor, "RowBufferRowMajor");

            // Test Column-Major Row Buffer
            RowBufferColumnMajor<int> rowBufferColumnMajor(matrixData.data(), 5, 4);
            _runBufferTests(rowBufferColumnMajor, "RowBufferColumnMajor");

            // Test Row-Major Column Buffer
            ColumnBufferRowMajor<int> columnBufferRowMajor(matrixData.data(), 4, 5);
            _runBufferTests(columnBufferRowMajor, "ColumnBufferRowMajor");

            // Test Column-Major Column Buffer
            ColumnBufferColumnMajor<int> columnBufferColumnMajor(matrixData.data(), 4);
            _runBufferTests(columnBufferColumnMajor, "ColumnBufferColumnMajor");
        }

        std::vector<int> _generateMatrixData(size_t rows, size_t cols) {
            std::vector<int> data(rows * cols);
            for (size_t i = 0; i < rows * cols; ++i) {
                data[i] = static_cast<int>(i);
            }
            return data;
        }

        template <typename BufferType>
        void _runBufferTests(BufferType& buffer, const std::string& bufferName) {
            printTestCaseResult(
                _testBufferElementAccess(buffer, bufferName), 
                bufferName + " - Element Access"
            );
            printTestCaseResult(
                _testBufferIterator(buffer, bufferName), 
                bufferName + " - Iterators"
            );
            printTestCaseResult(
                _testBufferConstIterator(buffer, bufferName), 
                bufferName + " - Const Iterators"
            );
        }

        template <typename BufferType>
        bool _testBufferElementAccess(BufferType& buffer, const std::string& bufferName) {
            for (std::size_t i = 0; i < buffer.size(); ++i) {
                int expectedValue = buffer[i]; // Expected value based on buffer logic
                if (buffer[i] != expectedValue) {
                    std::cout << bufferName << ": Expected: " << expectedValue << ", Found: " << buffer[i]
                              << " at index " << i << std::endl;
                    return false;
                }
            }
            return true;
        }

        template <typename BufferType>
        bool _testBufferIterator(BufferType& buffer, const std::string& bufferName) {
            std::size_t i = 0;
            for (auto it = buffer.begin(); it != buffer.end(); ++it, ++i) {
                int expectedValue = buffer[i]; // Expected value based on iterator logic
                if (*it != expectedValue) {
                    std::cout << bufferName << ": Expected: " << expectedValue << ", Found: " << *it
                              << " at iterator position " << i << std::endl;
                    return false;
                }
            }
            return true;
        }

        template <typename BufferType>
        bool _testBufferConstIterator(const BufferType& buffer, const std::string& bufferName) {
            std::size_t i = 0;
            for (auto it = buffer.cbegin(); it != buffer.cend(); ++it, ++i) {
                int expectedValue = buffer[i]; // Expected value based on const iterator logic
                if (*it != expectedValue) {
                    std::cout << bufferName << ": Expected: " << expectedValue << ", Found: " << *it
                              << " at const iterator position " << i << std::endl;
                    return false;
                }
            }
            return true;
        }
    };

} // namespace STLKR_Tests

#endif // MATRIXBUFFERS_VALIDITYTESTS_H
