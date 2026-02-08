/*
 * Copyright 2024-2026 Papas Christoforos Orestis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <Stalker/Mathematics/Integration/Integral.h>
#include <Stalker/Mathematics/MetaPolynomial.h>

namespace STLKR_Tests {


    class IntegrationTests : public STLKR_TestBase {
    public:
        explicit IntegrationTests()
            : STLKR_TestBase("Numeric Integration Tests"){

            }
              
        void runTest() override {
            _testExponential();
            _testPolynomial();
        }

    private:
        template <typename Array, size_t... Is>
        static constexpr std::array<double, sizeof...(Is)> _compute_exp_impl(const Array& x, std::index_sequence<Is...>) {
            return {{ std::exp(-x[Is])... }};
        }

        //Test from Numerical Methods: 4_Ολοκλήρωση.pdf - UTH, Prof. D. Valougeorgis
        //Integral of exp(-x) from 0.2 to 1.0 with 5 points
        //Trapezoidal:  0.4523
        //Simpson1:     0.4508
        //Simpson2: Not Applicable
        void _testExponential() {
            printSubtitle("Exponential Function: exp(-x)", T_Color::PATSIOURA_RED);
            constexpr size_t nPoints = 101;
            constexpr double a = 0.2;
            constexpr double b = 1.0;
            constexpr auto x = linspace<nPoints>(a, b);
            constexpr double stepSize = (b - a) / (nPoints - 1);
            constexpr auto f_x = _compute_exp_impl(x, std::make_index_sequence<nPoints>{});
            constexpr double expectedAnalytical = std::exp(-a) - std::exp(-b);

            auto resultTrapezoidal = Integral<IntegrationType::Trapezoidal, double>::evaluate(f_x.data(), f_x.size(), stepSize);
            auto resultSimpson1 = Integral<IntegrationType::Simpson1, double>::evaluate(f_x.data(), f_x.size(), stepSize);
            // auto resultSimpson2 = Integral<IntegrationType::Simpson2, double>::evaluate(f_x.data(), f_x.size(), stepSize);

            auto printerConfig = TestUtility::DoublePrinterConfig();
            printerConfig.precision = 6;            
            printerConfig.tolerance = 1E-3;

            TestUtility::compareDoubles(resultTrapezoidal, expectedAnalytical, "Trapezoidal", printerConfig);
            TestUtility::compareDoubles(resultSimpson1, expectedAnalytical, "Simpson1", printerConfig);
            // TestUtility::compareDoubles(resultSimpson2, expectedAnalytical, "Simpson2", printerConfig);

            constexpr auto ConstexprTrapezoidal = Integral<IntegrationType::Trapezoidal, double>::evaluate(f_x.data(), f_x.size(), stepSize);
            constexpr auto ConstexprSimpson1    = Integral<IntegrationType::Simpson1, double>::evaluate(f_x.data(), f_x.size(), stepSize);
            // constexpr auto ConstexprResultSimpson2 = Integral<IntegrationType::Simpson2, double>::evaluate(f_x.data(), f_x.size(), stepSize);

            static_assert(std::fabs(ConstexprTrapezoidal - expectedAnalytical) < 1E-4, "Trapezoidal");
            static_assert(std::fabs(ConstexprSimpson1 - expectedAnalytical) < 1E-8, "Simpson1");
            // static_assert(std::fabs(ConstexprResultSimpson2 - expectedAnalytical) < 1E-6, "Simpson2");
        }

        void _testPolynomial() {
            printSubtitle("Polynomial Function: P(x) = 100 + 3x + 0.5x² - 3x³ + (1/12)x⁴ + 20x⁵", T_Color::PATSIOURA_RED);
            constexpr auto coefficients = std::array<double, 6>{100, 3, 0.5, -3, 1.0/12.0, 20};
            constexpr MetaPolynomial<double, 5> p(coefficients);
            constexpr auto expectedIntegral = p.integral(0.0, 1.0);
            constexpr auto nPoints = 1001;
            constexpr auto stepSize = 1.0 / (nPoints - 1);
            
            constexpr auto f_x = p.sample<nPoints>(0.0, 1.0);

            auto printerConfig = TestUtility::DoublePrinterConfig();
            printerConfig.precision = 9;
            printerConfig.tolerance = 1E-4;

            constexpr auto constexprTrapezoidal = Integral<IntegrationType::Trapezoidal, double>::evaluate(f_x.data(), f_x.size(), stepSize);
            constexpr auto constexprSimpson1 = Integral<IntegrationType::Simpson1, double>::evaluate(f_x.data(), f_x.size(), stepSize);
            // constexpr auto constexprSimpson2 = Integral<IntegrationType::Simpson2, double>::evaluate(f_x.data(), f_x.size(), stepSize);

            auto resultTrapezoidal = Integral<IntegrationType::Trapezoidal, double>::evaluate(f_x.data(), f_x.size(), stepSize);
            auto resultSimpson1 = Integral<IntegrationType::Simpson1, double>::evaluate(f_x.data(), f_x.size(), stepSize);
            // auto resultSimpson2 = Integral<IntegrationType::Simpson2, double>::evaluate(f_x.data(), f_x.size(), stepSize);
            
            TestUtility::compareDoubles(resultTrapezoidal, expectedIntegral, "Trapezoidal", printerConfig);
            TestUtility::compareDoubles(resultSimpson1, expectedIntegral, "Simpson1", printerConfig);
            // TestUtility::compareDoubles(resultSimpson2, expectedIntegral, "Simpson2", printerConfig);

            static_assert(std::fabs(constexprTrapezoidal - expectedIntegral) < 1E-5, "Trapezoidal");
            static_assert(std::fabs(constexprSimpson1 - expectedIntegral) < 1E-11, "Simpson1");
            // static_assert(std::fabs(constexprSimpson2 - expectedIntegral) < 1E-6, "Simpson2");


        }

    };
} // namespace STLKR_Tests