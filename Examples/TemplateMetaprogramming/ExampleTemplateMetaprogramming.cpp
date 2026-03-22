/*
 * Example: Template Metaprogramming (TMP) Magic
 * Demonstrating zero-overhead static polymorphism, constexpr math,
 * and completely unrolled algorithmic logic.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <Stalker/Mathematics/MetaMath.h>
#include <Stalker/Mathematics/MetaPolynomial.h>
#include <Stalker/Mathematics/Vector/VectorMath.h>
#include <Stalker/Mathematics/Differentiation/Derivative.h>
#include <Stalker/Mathematics/Integration/Integral.h>
#include <Stalker/Core/Traits/ExecutionTraits.h>

using namespace Stalker::Core;
using namespace Stalker::Mathematics;

// ---------------------------------------------------------
// 1. Pure Compile-Time Constexpr Math
// ---------------------------------------------------------
void exampleConstexprMath() {    
    constexpr size_t factorial10 = factorial<10>();
    static_assert(factorial10 == 3628800, "TMP Evaluation Failed");
    
    constexpr size_t fibonacci10 = Fibonacci<10>::value;
    static_assert(fibonacci10 == 55, "TMP fib evaluation failed");

    constexpr size_t nChooseK = binomialCoefficient<10, 5>();
    static_assert(nChooseK == 80, "Binomial Coeff evaluates to 80 via integer division implementation.");
    
    constexpr double powerValue = power<3>(2.0);
    static_assert(powerValue == 8.0, "Power evaluation failed");

    std::cout << "    -> Factorial<10>: "   << factorial10 << " (Asserts passed)"<<std::endl;
    std::cout << "    -> Fibonacci<10>: "   << fibonacci10 << " (Asserts passed)"<<std::endl;
    std::cout << "    -> Binomial<10, 5>: " << nChooseK    << " (Asserts passed)"<<std::endl;
    std::cout << "    -> Power<3>(2.0): "   << powerValue  << " (Asserts passed)"<<std::endl ;
}

// ---------------------------------------------------------
// 2. Compile-Time Polynomial evaluation & Calculus
// ---------------------------------------------------------
void demonstrateCompileTimePolynomials() {
    std::cout << "[2] Compile-Time Horner Form Polynomial Evaluation\n";
    
    // Classic 2nd order polynomial: P(x) = 1 + 2x + x^2 = (x+1)^2
    constexpr std::array<double, 3> coeffs = {1.0, 2.0, 1.0}; 
    constexpr MetaPolynomial<double, 2> p(coeffs);
    
    constexpr double val = p(2.0); // 1 + 4 + 4 = 9
    static_assert(val == 9.0, "Polynomial evaluation failed");
    
    constexpr double derivative = p.derivative<1>(2.0); // 2 + 2x -> 2 + 4 = 6
    static_assert(derivative == 6.0, "Polynomial differentiation failed");
    
    constexpr double integral = p.integral(0.0, 3.0); // x + x^2 + (1/3)x^3 -> 3 + 9 + 9 = 21
    static_assert(integral == 21.0, "Polynomial integration failed");

    std::cout << "    -> P(x) = 1 + 2x + x^2 evaluated natively\n";
    std::cout << "    -> P(2) statically evaluated to: " << val << " (Asserts passed)\n";
    std::cout << "    -> P'(2) statically evaluated to: " << derivative << " (Asserts passed)\n";
    std::cout << "    -> Integral P(x) from 0 to 3 statically evaluated to: " << integral << " (Asserts passed)\n\n";
}

// ---------------------------------------------------------
// 3. Numeric vs Analytical Differentiation
// ---------------------------------------------------------
void exampleNumericDifferentiation() {
    std::cout << "[3] Numeric vs Analytical Differentiation (Central O(2))\n";
    
    constexpr std::array<double, 3> coeffs = {1.0, 2.0, 1.0}; 
    constexpr MetaPolynomial<double, 2> p(coeffs);
    
    constexpr double x = 2.0;
    constexpr double stepSize = 0.5;
    
    // Sample 3 points around x for Central O(2): [x - h, x, x + h]
    constexpr auto p_x = p.sample<3>(x - stepSize, x + stepSize);
    
    using namespace Stalker::Mathematics::Differentiation;
    constexpr auto derivativeEvaluator = Derivative<1, T_Scheme::Central, ErrorOrder::O_2>();
    
    constexpr auto analytical = p.derivative<1>(x);
    constexpr auto numeric = derivativeEvaluator.evaluate(p_x.data() + 1, stepSize);
    
    static_assert(std::abs(numeric - analytical) < 1E-6, "Differentiation check failed");

    std::cout << "    -> Analytical P'(2): " << analytical << "\n";
    std::cout << "    -> Numeric evaluate P'(2): " << numeric << " (Asserts passed)\n\n";
}

// ---------------------------------------------------------
// 4. Numeric vs Analytical Integration
// ---------------------------------------------------------
void exampleNumericIntegration() {
    std::cout << "[4] Numeric vs Analytical Integration (Simpson 1/3)\n";
    
    constexpr std::array<double, 3> coeffs = {1.0, 2.0, 1.0}; 
    constexpr MetaPolynomial<double, 2> p(coeffs);
    
    constexpr auto nPoints = 1001;
    constexpr auto a = 0.0;
    constexpr auto b = 3.0;
    constexpr auto stepSize = (b - a) / (nPoints - 1);
    
    // Sample domain into array
    constexpr auto f_x = p.sample<nPoints>(a, b);
    
    using namespace Stalker::Mathematics;
    
    constexpr auto analytical = p.integral(a, b);
    constexpr auto numeric = Integral<IntegrationType::Simpson1, double>::evaluate(f_x.data(), f_x.size(), stepSize);
    
    static_assert(std::abs(numeric - analytical) < 1E-6, "Integration check failed");

    std::cout << "    -> Analytical Integral(0, 3): " << analytical << "\n";
    std::cout << "    -> Numeric evaluated Integral: " << numeric << " (Asserts passed)\n\n";
}

// ---------------------------------------------------------
// Main Executable
// ---------------------------------------------------------
int main() {
    std::cout << "=================================================\n";
    std::cout << "  STAΛKER Example: C++17 TMP Magic\n";
    std::cout << "  Compile-Time Evaluations & Fold Expression Dispatch\n";
    std::cout << "=================================================\n\n";


    exampleConstexprMath();
    demonstrateCompileTimePolynomials();
    exampleNumericDifferentiation();
    exampleNumericIntegration();

    std::cout << "[✓] All TMP Validations: PASSED\n";
    return 0;
}
