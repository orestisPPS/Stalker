#ifndef STALKERMATRIX_ITERATOR_VALIDITYTESTS_H
#define STALKERMATRIX_ITERATOR_VALIDITYTESTS_H

#include "../STLKR_TestBase.h"
#include "../../DataStructures/Iterators/StrideIterator.h"  // Path to FixedStrideIterator
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
            // _testNonContiguousIterator();
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
            // int data[12] = {1, 2, 3, 4,
            //                 5, 6, 7, 8,
            //                 9, 10, 11, 12}; // Simulate 3x4 row-major
            // std::size_t totalRows = 3;

            // UpperTriangularMatrixIterator<int> it(data, totalRows); // Column in row-major
            // // Basic dereference
            // printTestCaseResult(*it == 1, "Non-contiguous iterator dereference operator (basic)");

            // //column 0 traversal
            // ++it; // Move to next row
            // printTestCaseResult(*it == 5, "Non-contiguous iterator pre-increment operator");
            // it++; // Move to next row
            // printTestCaseResult(*it == 9, "Non-contiguous iterator post-increment operator");

            // --it; // Move to previous row
            // printTestCaseResult(*it == 5, "Non-contiguous iterator pre-decrement operator");

            // it--; // Move to previous row
            // printTestCaseResult(*it == 1, "Non-contiguous iterator post-decrement operator");

            // // Advance
            // it.advance(4); // Skip a full column
            // printTestCaseResult(*it == 6, "Non-contiguous iterator advance operator");

            // // Distance
            // NonContiguousBlockIterator<int> it2(data, 4, totalRows, 1, 1); // 2nd row of 2nd column
            // printTestCaseResult(*it2 == 6, "Non-contiguous iterator dereference operator (Non - zero start)");
            // printTestCaseResult(it == it2, "Non-contiguous iterator equality");
            // printTestCaseResult(it <= it2, "Non-contiguous iterator less-than-or-equal");
            // // printTestCaseResult(it.distance(it2) == 1, "Non-contiguous iterator distance");
            // ++it; // Move to the first row of the next column
            // // Relational operators
            // printTestCaseResult(it != it2, "Non-contiguous iterator inequality");
            // printTestCaseResult(it2 < it, "Non-contiguous iterator less-than");
            // printTestCaseResult(it > it2, "Non-contiguous iterator greater-than");
}

    };

} // namespace STLKR_Tests

#endif // STALKERMATRIX_ITERATOR_VALIDITYTESTS_H
