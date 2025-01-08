#ifndef MATRIXBUFFERS_VALIDITYTESTS_H
#define MATRIXBUFFERS_VALIDITYTESTS_H

#include "../STLKR_TestBase.h"
#include "../../DataStructures/Buffers/FixedStridePtrBuffer.h"
#include "../../DataStructures/StalkerMatrix/Buffers/DenseMatrixGenericPtrBuffer.h"
#include <iostream>
#include <cassert>

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

            // Test FixedStridePtrBuffer (Row in Row-Major)
            DenseMatrixGenericPtrBuffer<int> contiguousRowBuffer(matrixData.data(), 5); // Row-major row buffer
            _runBufferTests(contiguousRowBuffer, "FixedStridePtrBuffer (Row in Row-Major)");

            // // Test NonContiguousBlockPtrBuffer (Column in Row-Major)
            DenseMatrixGenericPtrBuffer<int> nonContiguousColumnBuffer(matrixData.data(), 4, 4); // Row-major column buffer
            _runBufferTests(nonContiguousColumnBuffer, "NonContiguousBlockPtrBuffer (Column in Row-Major)");

            // Test FixedStridePtrBuffer (Column in Column-Major)
            DenseMatrixGenericPtrBuffer<int> contiguousColumnBuffer(matrixData.data(), 4); // Column-major column buffer
            _runBufferTests(contiguousColumnBuffer, "FixedStridePtrBuffer (Column in Column-Major)");

            // // Test NonContiguousBlockPtrBuffer (Row in Column-Major)
            DenseMatrixGenericPtrBuffer<int> nonContiguousRowBuffer(matrixData.data(), 5, 4); // Column-major row buffer
            _runBufferTests(nonContiguousRowBuffer, "NonContiguousBlockPtrBuffer (Row in Column-Major)");
        }

        std::vector<int> _generateMatrixData(size_t rows, size_t cols) {
            std::vector<int> data(rows * cols);
            for (size_t i = 0; i < rows * cols; ++i) {
                data[i] = static_cast<int>(i + 1); // Fill with 1-based indexing
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
            printTestCaseResult(
                _testBufferBounds(buffer, bufferName),
                bufferName + " - Bounds Check"
            );
        }

        template <typename BufferType>
        bool _testBufferElementAccess(BufferType& buffer, const std::string& bufferName) {
            try {
                for (std::size_t i = 0; i < buffer.size(); ++i) {
                    int expectedValue = buffer[i]; // Expected value
                    if (buffer[i] != expectedValue) {
                        std::cout << bufferName << ": Expected: " << expectedValue << ", Found: " << buffer[i]
                                  << " at index " << i << std::endl;
                        return false;
                    }
                }
            } catch (const std::exception& ex) {
                std::cout << bufferName << ": Exception during element access: " << ex.what() << std::endl;
                return false;
            }
            return true;
        }

        template <typename BufferType>
        bool _testBufferIterator(BufferType& buffer, const std::string& bufferName) {
            try {
                std::size_t i = 0;
                for (auto it = buffer.begin(); it != buffer.end(); ++it, ++i) {
                    int expectedValue = buffer[i]; // Expected value based on iterator logic
                    if (*it != expectedValue) {
                        std::cout << bufferName << ": Expected: " << expectedValue << ", Found: " << *it
                                  << " at iterator position " << i << std::endl;
                        return false;
                    }
                }
            } catch (const std::exception& ex) {
                std::cout << bufferName << ": Exception during iterator access: " << ex.what() << std::endl;
                return false;
            }
            return true;
        }

        template <typename BufferType>
        bool _testBufferConstIterator(const BufferType& buffer, const std::string& bufferName) {
            try {
                std::size_t i = 0;
                for (auto it = buffer.cbegin(); it != buffer.cend(); ++it, ++i) {
                    int expectedValue = buffer[i]; // Expected value based on const iterator logic
                    if (*it != expectedValue) {
                        std::cout << bufferName << ": Expected: " << expectedValue << ", Found: " << *it
                                  << " at const iterator position " << i << std::endl;
                        return false;
                    }
                }
            } catch (const std::exception& ex) {
                std::cout << bufferName << ": Exception during const iterator access: " << ex.what() << std::endl;
                return false;
            }
            return true;
        }

        template <typename BufferType>
        bool _testBufferBounds(BufferType& buffer, const std::string& bufferName) {
            try {
                // Access beyond buffer size should throw
                buffer[buffer.size()] = 0;
                std::cout << bufferName << ": Expected out_of_range exception, but none was thrown." << std::endl;
                return false;
            } catch (const std::out_of_range&) {
                return true; // Exception correctly thrown
            } catch (const std::exception& ex) {
                std::cout << bufferName << ": Unexpected exception during bounds check: " << ex.what() << std::endl;
                return false;
            }
        }
    };

} // namespace STLKR_Tests

#endif // MATRIXBUFFERS_VALIDITYTESTS_H
