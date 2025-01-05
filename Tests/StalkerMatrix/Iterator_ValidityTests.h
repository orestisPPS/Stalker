#ifndef STALKERMATRIX_ITERATOR_VALIDITYTESTS_H
#define STALKERMATRIX_ITERATOR_VALIDITYTESTS_H

#include "../STLKR_TestBase.h"
#include "../../DataStructures/Iterators/FixedStrideIteratorBase.h"  // Path to FixedStrideIterator
#include "../../DataStructures/Iterators/LinearStrideIterator.h"  // Path to LinearStrideIterator
#include <cmath>  // For std::abs in floating-point comparisons

namespace STLKR_Tests {

    class MatrixBlockIteratorTest : public STLKR_TestBase {
    public:
        explicit MatrixBlockIteratorTest() 
            : STLKR_TestBase("Matrix Block Iterator Test") {}

        void runTest() override {
            _testContiguousIteratorBasic();
            _testContiguousIteratorArithmetic();
            _testContiguousIteratorComparison();
            _testContiguousIteratorRandomAccess();
            _testNonContiguousIterator();
        }

    private:

        void _testContiguousIteratorBasic() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            FixedStrideIterator<int> it(data, 2); // Stride of 2

            printTestCaseResult(*it == 1, "Contiguous iterator dereference operator (basic)");
            ++it;
            printTestCaseResult(*it == 3, "Contiguous iterator pre-increment operator");
            it++;
            printTestCaseResult(*it == 5, "Contiguous iterator post-increment operator");
            --it;
            printTestCaseResult(*it == 3, "Contiguous iterator pre-decrement operator");
            it--;
            printTestCaseResult(*it == 1, "Contiguous iterator post-decrement operator");
        }

        void _testContiguousIteratorArithmetic() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            FixedStrideIterator<int> it(data, 2); // Stride of 2

            FixedStrideIterator<int> it2 = it + 3;
            printTestCaseResult(*it2 == 7, "Contiguous iterator addition operator");
            it += 2;
            printTestCaseResult(*it == 5, "Contiguous iterator compound addition operator");

            FixedStrideIterator<int> it3 = it - 2;
            printTestCaseResult(*it3 == 1, "Contiguous iterator subtraction operator");
            it -= 1;
            printTestCaseResult(*it == 3, "Contiguous iterator compound subtraction operator");

            printTestCaseResult(it3 - it == -1, "Contiguous iterator distance between iterators");
        }

        void _testContiguousIteratorComparison() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            FixedStrideIterator<int> it1(data, 2);
            FixedStrideIterator<int> it2 = it1 + 2;

            printTestCaseResult(it1 != it2, "Contiguous iterator inequality operator");
            printTestCaseResult(it1 < it2, "Contiguous iterator less-than operator");
            printTestCaseResult(it2 > it1, "Contiguous iterator greater-than operator");
            printTestCaseResult(it1 <= it2, "Contiguous iterator less-than-or-equal operator");
            printTestCaseResult(it2 >= it1, "Contiguous iterator greater-than-or-equal operator");
        }

        void _testContiguousIteratorRandomAccess() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            FixedStrideIterator<int> it(data, 2);

            printTestCaseResult(it[3] == 7, "Contiguous iterator random access operator");
        }

    void _testNonContiguousIterator() {
    // Simulated data for a 4x4 upper triangular matrix in row-major order:
    // Row-major storage: 0 1 2 3
    //                    0 5 6 7
    //                    0 0 10 11
    //                    0 0 0 15
    std::size_t totalRows = 4;
    std::size_t targetColumn = 3;
    auto upperTriangularData = new int[10] {0, 1, 2, 3, 5, 6, 7, 10, 11, 15};

    // Iterator for Column 4 in row-major storage:
    LinearStrideIterator<int, false> it(upperTriangularData + targetColumn, totalRows - 1); // Column 1 starts at index 1 with a stride of totalRows - 1
    printTestCaseResult(*it == 3, "Upper triangular row-major: dereference column 3, row 0");

    // Pre-increment
    ++it; // Move to next row
    printTestCaseResult(*it == 7, "Upper triangular row-major: pre-increment column 3 to row 1");

    // Post-increment
    it++;
    printTestCaseResult(*it == 11, "Upper triangular row-major: post-increment column 3 to row 2");

    // Pre-decrement
    --it; // Move back one row
    printTestCaseResult(*it == 7, "Upper triangular row-major: pre-decrement column 3 to row 1");

    // Post-decrement
    it--;
    printTestCaseResult(*it == 3, "Upper triangular row-major: post-decrement column 3 to row 0");

    // Random access and distance checks
    it += 2; // Jump to the third row
    printTestCaseResult(*it == 11, "Upper triangular row-major: compound addition operator");

    LinearStrideIterator<int, false> it2 = it - 2; // Go back to the first row
    printTestCaseResult(*it2 == 3, "Upper triangular row-major: compound subtraction operator");
    printTestCaseResult(it2 - it == -2, "Upper triangular row-major: distance between iterators");

    // Comparison operators
    printTestCaseResult(it2 < it, "Upper triangular row-major: less-than operator");
    printTestCaseResult(it > it2, "Upper triangular row-major: greater-than operator");
    printTestCaseResult(!(it == it2), "Upper triangular row-major: equal operator");
    printTestCaseResult(it2 <= it, "Upper triangular row-major: less-than-or-equal operator");
    printTestCaseResult(it >= it2, "Upper triangular row-major: greater-than-or-equal operator");

}



    };

} // namespace STLKR_Tests

#endif // STALKERMATRIX_ITERATOR_VALIDITYTESTS_H
