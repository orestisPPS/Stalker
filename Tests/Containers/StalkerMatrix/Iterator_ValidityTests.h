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
            Printers::printSubtitle("Contiguous Iterator", T_Color::PATSIOURA_RED);
            _testContiguousIteratorBasic();
            _testContiguousIteratorArithmetic();
            _testContiguousIteratorComparison();
            _testContiguousIteratorRandomAccess();
            Printers::printSubtitle("Non-Contiguous Iterator (Column in Upper Triangular Row Major Matrix)", T_Color::PATSIOURA_RED);
            _testNonContiguousIterator();
        }

    private:

        void _testContiguousIteratorBasic() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            FixedStrideIterator<int> it(data, 2); // Stride of 2

            TestUtility::compareValues<int>(*it, 1, "Dereference operator (basic)");
            ++it;
            TestUtility::compareValues<int>(*it, 3, "Pre-increment operator");
            it++;
            TestUtility::compareValues<int>(*it, 5, "Post-increment operator");
            --it;
            TestUtility::compareValues<int>(*it, 3, "Pre-decrement operator");
            it--;
            TestUtility::compareValues<int>(*it, 1, "Post-decrement operator");
        }

        void _testContiguousIteratorArithmetic() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            FixedStrideIterator<int> it(data, 2); // Stride of 2

            FixedStrideIterator<int> it2 = it + 3;
            TestUtility::compareValues<int>(*it2, 7, "Addition operator");
            it += 2;
            TestUtility::compareValues<int>(*it, 5, "Compound addition operator");

            FixedStrideIterator<int> it3 = it - 2;
            TestUtility::compareValues<int>(*it3, 1, "Subtraction operator");
            it -= 1;
            TestUtility::compareValues<int>(*it, 3, "Compound subtraction operator");

            TestUtility::compareValues<int>(it3 - it, -1, "Distance between iterators");
        }

        void _testContiguousIteratorComparison() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            FixedStrideIterator<int> it1(data, 2);
            FixedStrideIterator<int> it2 = it1 + 2;

            TestUtility::compareValues<bool>(it1 != it2, true, "!=");
            TestUtility::compareValues<bool>(it1 < it2, true, "<");
            TestUtility::compareValues<bool>(it2 > it1, true, ">");
            TestUtility::compareValues<bool>(it1 <= it2, true, "<=");
            TestUtility::compareValues<bool>(it2 >= it1, true, ">=");
        }

        void _testContiguousIteratorRandomAccess() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            FixedStrideIterator<int> it(data, 2);

            TestUtility::compareValues<int>(it[3], 7, "random access operator");
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
    TestUtility::compareValues<int>(*it, 3, "Dereference column 3, row 0");

    // Pre-increment
    ++it; // Move to next row
    TestUtility::compareValues<int>(*it, 7, "Pre-increment column 3 to row 1");

    // Post-increment
    it++;
    TestUtility::compareValues<int>(*it, 11, "Post-increment column 3 to row 2");

    // Pre-decrement
    --it; // Move back one row
    TestUtility::compareValues<int>(*it, 7, "Pre-decrement column 3 to row 1");

    // Post-decrement
    it--;
    TestUtility::compareValues<int>(*it, 3, "Post-decrement column 3 to row 0");

    // Random access and distance checks
    it += 2; // Jump to the third row
    TestUtility::compareValues<int>(*it, 11, "Compound addition operator");

    LinearStrideIterator<int, false> it2 = it - 2; // Go back to the first row
    TestUtility::compareValues<int>(*it2, 3, "Compound subtraction operator");
    TestUtility::compareValues<int>(it2 - it, -2, "Distance between iterators");

    // Comparison operators
    TestUtility::compareValues<bool>(it2 < it, true, "<");
    TestUtility::compareValues<bool>(it > it2, true, ">");
    TestUtility::compareValues<bool>(!(it == it2), true, "!=");
    TestUtility::compareValues<bool>(it2 <= it, true, "<=");
    TestUtility::compareValues<bool>(it >= it2, true, ">=");

}



    };

} // namespace STLKR_Tests

#endif // STALKERMATRIX_ITERATOR_VALIDITYTESTS_H
