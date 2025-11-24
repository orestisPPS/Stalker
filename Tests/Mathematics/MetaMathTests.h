#pragma once

#include "../STLKR_TestBase.h"
#include <Stalker/Mathematics/Operations/MathOperationsMeta.h>
#include <Stalker/Mathematics/MetaMath.h>
#include <Stalker/Mathematics/MetaPolynomial.h>
#include "../TestUtility.h"

namespace STLKR_Tests {

    template <typename T>
    class MetaMathTests : public STLKR_TestBase {
    public:
        explicit MetaMathTests()
            : STLKR_TestBase("Template Metaprogramming Mathematics Tests"){

            }
              
        void runTest() override {
            printTitle("Basic Mathematics", "-", T_Color::WHITE);
            _testPower();
            _testFactorial();
            _testFallingFactorial();
            _testBinomialCoefficient();
            _testFibonacci();
            printTitle("Unrolled MathOperationsMeta", "-", T_Color::WHITE);
            _testAdd();
            _testSubtract();
            _testMultiply();
            _testDivide();
            _testDotProduct();
            _testSum();
            _testPartialSum();

            // _testMetaPolynomial();
        }

        static void _testPower() {
            constexpr auto result = power<5>(2);
            constexpr auto expected = 32;
            TestUtility::compareValues<int>(result, expected, "Power (2^5)");
            static_assert(result == expected, "Power (2^5) Test Failed");
        }

        static void _testFactorial() {
            constexpr auto result = factorial<5>();
            constexpr auto expected = 120;
            TestUtility::compareValues<int>(result, expected, "Factorial (5!)");
            static_assert(result == expected, "Factorial (5!) Test Failed");
        }

        static void _testFallingFactorial() {
            constexpr auto result = fallingFactorial<5, 2>();
            constexpr auto expected = 20;
            TestUtility::compareValues<int>(result, expected, "Falling Factorial (5! / 3!)");
            static_assert(result == expected, "Falling Factorial (5! / 3!) Test Failed");
        }

        static void _testBinomialCoefficient() {
            constexpr auto result = binomialCoefficient<5, 2>();
            constexpr auto expected = 10;
            TestUtility::compareValues<int>(result, expected, "Binomial Coefficient (5 choose 2)");
            static_assert(result == expected, "Binomial Coefficient (5 choose 2) Test Failed");
        }
        
        void _testFibonacci() {
            constexpr auto result = Fibonacci<5>::value;
            constexpr auto expected = 5;
            TestUtility::compareValues<int>(result, expected, "Fibonacci (Fib(5))");
            static_assert(result == expected, "Fibonacci (Fib(5)) Test Failed");
        }

        void _testAdd() {
            constexpr std::array<double, size()> data1 = {0, 1, 2, 3, 4};
            constexpr std::array<double, size()> data2 = {5, 6, 7, 8, 9};
            std::array<double, size()> result = {};
            // MathOperationsMeta::add<double, size()>(data1.data(), data2.data(), result.data(), 1, 1);
            MathOperationsMeta::add<double, size()>(size(), data1.data(), data2.data(), result.data());
            constexpr std::array<double, size()> expected = {5, 7, 9, 11, 13};
            TestUtility::compareVectors<double>(result.data(), expected.data(), result.size(), "Addition of Two Arrays");
            // static_assert(result == expected, "Addition of Two Arrays Test Failed");
        }
        
        static constexpr size_t size() {
            return 5;
        };

        void _testSubtract() {
            constexpr std::array<double, size()> data1 = {0, 1, 2, 3, 4};
            constexpr std::array<double, size()> data2 = {5, 6, 7, 8, 9};
            std::array<double, size()> result  = {};
            // MathOperationsMeta::subtract<double, size()>(data1.data(), data2.data(), result.data(), 1, 1);
            MathOperationsMeta::subtract<double, size()>(size(), data1.data(), data2.data(), result.data());
            constexpr std::array<double, size()> expected = {-5, -5, -5, -5, -5};
            TestUtility::compareVectors<double>(result.data(), expected.data(), result.size(), "Subtraction of Two Arrays");
            // static_assert(result == expected, "Subtraction of Two Arrays Test Failed");
        }

        void _testMultiply() {
            constexpr std::array<double, size()> data1 = {0, 1, 2, 3, 4};
            constexpr std::array<double, size()> data2 = {5, 6, 7, 8, 9};
            std::array<double, size()> result = {};
            // MathOperationsMeta::multiply<double, size()>(data1.data(), data2.data(), result.data(), 1, 1);
            MathOperationsMeta::multiply<double, size()>(size(), data1.data(), data2.data(), result.data());
            constexpr std::array<double, size()> expected = {0, 6, 14, 24, 36};
            TestUtility::compareVectors<double>(result.data(), expected.data(), result.size(), "Multiplication of Two Arrays");
            // static_assert(result == expected, "Multiplication of Two Arrays Test Failed");
        }

        void _testDivide() {
            constexpr std::array<double, size()> data1 = {0, 1, 2, 3, 4};
            constexpr std::array<double, size()> data2 = {5, 6, 7, 8, 9};
            std::array<double, size()> result = {};
            // MathOperationsMeta::divide<double, size()>(data1.data(), data2.data(), result.data(), 1, 1);
            MathOperationsMeta::divide<double, size()>(size(), data1.data(), data2.data(), result.data());
            constexpr std::array<double, size()> expected = {0, 1.0 / 6, 2.0 / 7, 3.0 / 8, 4.0 / 9};
            TestUtility::compareVectors<double>(result.data(), expected.data(), result.size(), "Division of Two Arrays");
            // static_assert(result == expected, "Division of Two Arrays Test Failed");
        }

        void _testDotProduct() {
            constexpr std::array<double, size()> data1 = {0, 1, 2, 3, 4};
            constexpr std::array<double, size()> data2 = {5, 6, 7, 8, 9};
            constexpr auto result = MathOperationsMeta::dot<double, size()>(size(), data1.data(), data2.data());
            constexpr auto expected = 80;
            TestUtility::compareValues<double>(result, expected, "Dot Product of Two Arrays");
            static_assert(result == expected, "Dot Product of Two Arrays Test Failed");
        }

        void _testSum() {
            constexpr std::array<double, size()> data = {0, 1, 2, 3, 4};
            constexpr auto result = MathOperationsMeta::sum<double, size()>(size(), data.data());
            constexpr auto expected = 10;
            TestUtility::compareValues<double>(result, expected, "Sum of an Array");
            static_assert(result == expected, "Sum of an Array Test Failed");
        }

        static void _testPartialSum() {
            constexpr std::array<double, size()> data = {0, 1, 2, 3, 4};

            {
                constexpr auto result = MathOperationsMeta::sum<double, size()>(size(), data.data());
                constexpr auto expected = 10.0;
                TestUtility::compareValues<double>(result, expected, "Partial Sum 0..4");
                static_assert(result == expected, "Partial Sum 0..4 Test Failed");
            }

            {
                constexpr auto result = MathOperationsMeta::sum<double, 3>(3, data.data() + 1);
                constexpr auto expected = 6.0;
                TestUtility::compareValues<double>(result, expected, "Partial Sum 1..3");
                static_assert(result == expected, "Partial Sum 1..3 Test Failed");
            }

            {
                constexpr auto result = MathOperationsMeta::sum<double, 1>(1, data.data() + 2);
                constexpr auto expected = 2.0;
                TestUtility::compareValues<double>(result, expected, "Partial Sum 2..2");
                static_assert(result == expected, "Partial Sum 2..2 Test Failed");
            }
        }

        static void _testMetaPolynomial(){
            printTitle("Constexpr Polynomial Tests", "-", T_Color::WHITE);

            // Quadratic Polynomial P(x) = 3x² + 2x + 1
            printSubtitle("Quadratic Polynomial P(x) = 3x² + 2x + 1 (x=2.0)", T_Color::PATSIOURA_RED);
            constexpr std::array<double, 3> coefficients = {1, 2, 3};
            constexpr MetaPolynomial<double, 2> p1(coefficients);

            constexpr double x = 2.0;
            constexpr auto expected = 3 * (x * x) + 2 * x + 1;  // Expected: 3(2²) + 2(2) + 1 = 17

            constexpr auto result_naive = p1.evaluateNaive(x);
            TestUtility::compareValues<double>(result_naive, expected, "Evaluation (Naive)");
            static_assert(result_naive == expected, "Evaluation (Naive) Test Failed");

            constexpr auto result_horner = p1(x);
            TestUtility::compareValues<double>(result_horner, expected, "Evaluation (Horner)");
            static_assert(result_horner == expected, "Evaluation (Horner) Test Failed");

            // First Derivative P'(x) = 6x + 2
            constexpr auto expected_derivative1 = 6 * x + 2;  // Expected: 6(2) + 2 = 14
            constexpr auto result_derivative1 = p1.derivative<1>(x);
            TestUtility::compareValues<double>(result_derivative1, expected_derivative1, "Derivative 1");
            static_assert(result_derivative1 == expected_derivative1, "Derivative 1 Test Failed");

            // Second Derivative P''(x) = 6 (constant)
            constexpr auto expected_derivative2 = 6;  // Expected: 6
            constexpr auto result_derivative2 = p1.derivative<2>(x);
            TestUtility::compareValues<double>(result_derivative2, expected_derivative2, "Derivative 2");
            static_assert(result_derivative2 == expected_derivative2, "Derivative 2 Test Failed");

            // Integral of P(x) = 3x² + 2x + 1
            constexpr auto result_integral = p1.integral(1.0, 2.0);
            constexpr auto expected_integral = 11.0;
            TestUtility::compareValues<double>(result_integral, expected_integral, "Integral (x=1.0 to x=2.0)");
            static_assert(result_integral == expected_integral, "Integral (x=1.0 to x=2.0) Test Failed");


            //Higher Order Polynomial P(x) = 4x^4 - 3x^3 + 2x^2 - x + 5
            printSubtitle("Polynomial of Order 4: P(x) = 4x⁴ - 3x³ + 2x² - x + 5 (x=2.0)", T_Color::PATSIOURA_RED);
            constexpr std::array<double, size()> coefficients_high = {5, -1, 2, -3, 4};  // Represents 4x^4 - 3x^3 + 2x^2 - x + 5
            constexpr MetaPolynomial<double, 4> p2(coefficients_high);

            constexpr auto result_high = p2.evaluateNaive(x);
            constexpr auto expected_high = 4 * (x * x * x * x) - 3 * (x * x * x) + 2 * (x * x) - x + 5;
            TestUtility::compareValues<double>(result_high, expected_high, "Evaluation (Naive)");
            static_assert(result_high == expected_high, "Evaluation (Naive) Test Failed");

            constexpr auto result_high_horner = p2(x);
            TestUtility::compareValues<double>(result_high_horner, expected_high, "Evaluation (Horner)");
            static_assert(result_high_horner == expected_high, "Evaluation (Horner) Test Failed");
            
            //First Derivative of P(x) = 16x^3 - 9x^2 + 4x - 1
            constexpr auto result_high_derivative1 = p2.derivative<1>(x);
            constexpr auto expected_high_derivative1 = 16 * (x * x * x) - 9 * (x * x) + 4 * x - 1;
            TestUtility::compareValues<double>(result_high_derivative1, expected_high_derivative1, "Derivative 1");
            static_assert(result_high_derivative1 == expected_high_derivative1, "Derivative 1");

            // Second Derivative of P(x) = 48x² - 18x + 4
            constexpr auto result_high_derivative2 = p2.derivative<2>(x);
            constexpr auto expected_high_derivative2 = 48 * (x * x) - 18 * x + 4;
            TestUtility::compareValues<double>(result_high_derivative2, expected_high_derivative2, "Derivative 2");
            static_assert(result_high_derivative2 == expected_high_derivative2, "Derivative 2 Test Failed");

            // Integral of P(x) = 4x^4 - 3x^3 + 2x^2 - x + 5 from x=1.0 to x=2.0 
            constexpr auto result_integral2 = p2.integral(1.0, 2.0);
            constexpr auto expected_integral2 = 1303.0/60.0;
            TestUtility::compareValues<double>(result_integral2, expected_integral2, "Integral (x=1.0 to x=2.0)", 1e-9);
            // static_assert(result_integral2 == expected_integral2, "Integral (x=1.0 to x=2.0) Test Failed");
        }
    };
} // namespace STLKR_Tests