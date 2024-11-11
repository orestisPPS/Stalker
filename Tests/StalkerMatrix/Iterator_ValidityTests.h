#ifndef STALKERMATRIX_ITERATOR_VALIDITYTESTS_H
#define STALKERMATRIX_ITERATOR_VALIDITYTESTS_H

#include "../STLKR_TestBase.h"
#include "../StalkerMatrix/MatrixIterators.h" // Path to your iterator implementation
#include <cmath>  // For std::abs in floating-point comparisons

namespace STLKR_Tests {

    class MatrixStrideIteratorTest : public STLKR_TestBase {
    public:
        explicit MatrixStrideIteratorTest() 
            : STLKR_TestBase("Matrix Stride Iterator Test") {}

        void runTest() override {
            _testStrideIteratorBasic();
            _testStrideIteratorArithmetic();
            _testStrideIteratorComparison();
            _testStrideIteratorRandomAccess();
            _testStrideConstIterator();
        }

    private:

        void _testStrideIteratorBasic() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            MatrixStrideIterator<int> it(data, 2); // Stride of 2

            printTestCaseResult(*it == 1, "Dereference operator (basic)");
            ++it;
            printTestCaseResult(*it == 3, "Pre-increment operator");
            it++;
            printTestCaseResult(*it == 5, "Post-increment operator");
            --it;
            printTestCaseResult(*it == 3, "Pre-decrement operator");
            it--;
            printTestCaseResult(*it == 1, "Post-decrement operator");
        }

        void _testStrideIteratorArithmetic() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            MatrixStrideIterator<int> it(data, 2); // Stride of 2

            MatrixStrideIterator<int> it2 = it + 3;
            printTestCaseResult(*it2 == 7, "Addition operator");
            it += 2;
            printTestCaseResult(*it == 5, "Compound addition operator");

            MatrixStrideIterator<int> it3 = it - 2;
            printTestCaseResult(*it3 == 1, "Subtraction operator");
            it -= 1;
            printTestCaseResult(*it == 3, "Compound subtraction operator");

            printTestCaseResult(it3 - it == -1, "Distance between iterators");
        }

        void _testStrideIteratorComparison() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            MatrixStrideIterator<int> it1(data, 2);
            MatrixStrideIterator<int> it2 = it1 + 2;

            printTestCaseResult(it1 != it2, "Inequality operator");
            printTestCaseResult(it1 < it2, "Less-than operator");
            printTestCaseResult(it2 > it1, "Greater-than operator");
            printTestCaseResult(it1 <= it2, "Less-than-or-equal operator");
            printTestCaseResult(it2 >= it1, "Greater-than-or-equal operator");
        }

        void _testStrideIteratorRandomAccess() {
            int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            MatrixStrideIterator<int> it(data, 2);

            printTestCaseResult(it[3] == 7, "Random access operator");
            printTestCaseResult(!(it[-1] == -1), "Negative offset access");
        }

        void _testStrideConstIterator() {
            const int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            MatrixStrideIterator<const int> constIt(data, 2);

            printTestCaseResult(*constIt == 1, "Const dereference operator");
            ++constIt;
            printTestCaseResult(*constIt == 3, "Const pre-increment operator");

            auto constIt2 = constIt + 3;
            printTestCaseResult(*constIt2 == 9, "Const addition operator");
        }
    };

} // namespace STLKR_Tests



#endif // STALKERMATRIX_ITERATOR_VALIDITYTESTS_H