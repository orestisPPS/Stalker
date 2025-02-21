#ifndef MATHEMATICALEXPRESSIONS_VALIDITYTESTS_H
#define MATHEMATICALEXPRESSIONS_VALIDITYTESTS_H

#include "../STLKR_TestBase.h"
#include "../StalkerMathematics/MathematicalExpressions.h"
#include "../TestUtility.h"

namespace STLKR_Tests {

    template <typename T>
    class MathematicalExpressions_ValidityTests : public STLKR_TestBase {
    public:
        explicit MathematicalExpressions_ValidityTests()
            : STLKR_TestBase("Template Mathematical Expression Tests"){
                _a = {1, 1, 1, 1, 1};
                _b = {2, 2, 2, 2, 2};
                _c = {0, 1, 2, 3, 4};
            }
              
        void runTest() override {
            Printers::printTitle(this->_testName, "=", ColourType::BRIGHT_WHITE);
            // _testAddition2Vectors();
            _testScalarMultiplicationVector();
        }

    private:
        std::vector<T> _a;
        std::vector<T> _b;
        std::vector<T> _c;

        // void _testAddition2Vectors() {
        //     auto addExpression = _a + _b;
        //     auto expected = std::vector<T>{3, 3, 3, 3, 3};
        //     auto result = std::vector<T>(addExpression.size());
        //     evaluate(addExpression, result);
        //     TestUtility::printComparisonTable(result, expected, "Addition of 2 Vectors (a + b)", true);
        // }

        // void _testSubtraction2Vectors() {
        //     auto subtractExpression = _a - _b;
        //     auto expected = std::vector<T>{-1, -1, -1, -1, -1};
        //     auto result = std::vector<T>(subtractExpression.size());
        //     evaluate(subtractExpression, result);
        //     TestUtility::printComparisonTable(result, expected, "Subtraction of 2 Vectors (a - b)", true);
        // }

        // void _testMultiplication2Vectors() {
        //     auto multiplyExpression = _a * _b;
        //     auto expected = std::vector<T>{2, 2, 2, 2, 2};
        //     auto result = std::vector<T>(multiplyExpression.size());
        //     evaluate(multiplyExpression, result);
        //     TestUtility::printComparisonTable(result, expected, "Multiplication of 2 Vectors (a * b)", true);
        // }

        // void _testNegationVector() {
        //     auto negateExpression = -_a;
        //     auto expected = std::vector<T>{-1, -1, -1, -1, -1};
        //     auto result = std::vector<T>(negateExpression.size());
        //     evaluate(negateExpression, result);
        //     TestUtility::printComparisonTable(result, expected, "Negation of a Vector (-a)", true);
        // }

        void _testScalarMultiplicationVector() {
            auto scalar = 2;
            auto multiplyExpression =  scalar * _a ;
            auto expected = std::vector<T>{2, 2, 2, 2, 2};
            auto result = std::vector<T>(expected.size());
            evaluate(multiplyExpression, result, result.size());
            TestUtility::printComparisonTable(result, expected, "Scalar Multiplication of a Vector (a * scalar)", true);
        }

        // void _testComplexExpression(){
        //     auto expression = - (_a + _b) - 2 * _c;
        //     auto expected = std::vector<T>{ -3, -5, -7, -9, -11};
        //     auto result = std::vector<T>(expression.size());
        //     evaluate(expression, result);
        //     TestUtility::printComparisonTable(result, expected, "Complex Expression ((a + b) * c)", true);
        // }

    };
} // namespace STLKR_Tests

#endif // MATHEMATICALEXPRESSIONS_VALIDITYTESTS_H