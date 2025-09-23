#ifndef DIFFERENTIATION_VALIDITYTESTS_H
#define DIFFERENTIATION_VALIDITYTESTS_H

#include <Stalker/Mathematics/Differentiation/Derivative.h>
#include <Stalker/Mathematics/MetaPolynomial.h>
#include "../STLKR_TestBase.h"
#include "../TestUtility.h"

namespace STLKR_Tests {
    using namespace Stalker::Mathematics::Differentiation;

    class Differentiation_ValidityTests : public STLKR_TestBase {
    public:
        explicit Differentiation_ValidityTests()
            : STLKR_TestBase("Numeric Differentiation Tests"){
                
            }
              
        void runTest() override {
            //Central
            printTitle("1st Derivative", "-", ColourType::WHITE);

            printSubtitle("Central Finite Difference Scheme", ColourType::PATSIOURA_RED);
            _testPolynomials<1, SchemeType::Central, ErrorOrder::O_2>();
            _testPolynomials<1, SchemeType::Central, ErrorOrder::O_4>();
            _testPolynomials<1, SchemeType::Central, ErrorOrder::O_6>();
            //Forward-
            printSubtitle("Forward Finite Difference Scheme", ColourType::PATSIOURA_RED);
            _testPolynomials<1, SchemeType::Forward, ErrorOrder::O_1>();
            _testPolynomials<1, SchemeType::Forward, ErrorOrder::O_2>();
            _testPolynomials<1, SchemeType::Forward, ErrorOrder::O_3>();
            _testPolynomials<1, SchemeType::Forward, ErrorOrder::O_4>();
            _testPolynomials<1, SchemeType::Forward, ErrorOrder::O_5>();
            //Backward
            printSubtitle("Backward Finite Difference Scheme", ColourType::PATSIOURA_RED);
            _testPolynomials<1, SchemeType::Backward, ErrorOrder::O_1>();
            _testPolynomials<1, SchemeType::Backward, ErrorOrder::O_2>();
            _testPolynomials<1, SchemeType::Backward, ErrorOrder::O_3>();
            _testPolynomials<1, SchemeType::Backward, ErrorOrder::O_4>();
            _testPolynomials<1, SchemeType::Backward, ErrorOrder::O_5>();

            printTitle("2nd Derivative", "-", ColourType::WHITE);
            
            printSubtitle("Central Finite Difference Scheme", ColourType::PATSIOURA_RED);
            _testPolynomials<2, SchemeType::Central, ErrorOrder::O_2>();
            _testPolynomials<2, SchemeType::Central, ErrorOrder::O_4>();
            _testPolynomials<2, SchemeType::Central, ErrorOrder::O_6>();
            //Forward-
            printSubtitle("Forward Finite Difference Scheme", ColourType::PATSIOURA_RED);
            _testPolynomials<2, SchemeType::Forward, ErrorOrder::O_2>();
            _testPolynomials<2, SchemeType::Forward, ErrorOrder::O_3>();
            _testPolynomials<2, SchemeType::Forward, ErrorOrder::O_4>();
            _testPolynomials<2, SchemeType::Forward, ErrorOrder::O_5>();
            //Backward
            printSubtitle("Backward Finite Difference Scheme", ColourType::PATSIOURA_RED);
            _testPolynomials<2, SchemeType::Backward, ErrorOrder::O_2>();
            _testPolynomials<2, SchemeType::Backward, ErrorOrder::O_3>();
            _testPolynomials<2, SchemeType::Backward, ErrorOrder::O_4>();
            _testPolynomials<2, SchemeType::Backward, ErrorOrder::O_5>();
        }

    private:

        template<size_t DerivativeOrder, SchemeType Scheme, ErrorOrder Error, bool StaticAssert = false>
        void _testPolynomials(){
            constexpr auto x = 2.0;
            constexpr auto stepSize = 5E-1;

            constexpr auto coefficients1 = std::array<double, 3>{1, 2, 3};
            constexpr auto p = MetaPolynomial<double, 2>(coefficients1);
            // x = linspace<nPoints>(x - 8 * stepSize, x + 8 * stepSize, 17);

            // constexpr auto coefficients1 = std::array<double, 6>{5, 2, 0.1, 3.5, 5, 7};
            // constexpr auto p = MetaPolynomial<double, 5>(coefficients1);

            constexpr std::array<double, 17> p_x = {
                p(x - 8 * stepSize), p(x - 7 * stepSize), p(x - 6 * stepSize),
                p(x - 5 * stepSize), p(x - 4 * stepSize), p(x - 3 * stepSize),
                p(x - 2 * stepSize), p(x - 1 * stepSize),
                p(x),
                p(x + 1 * stepSize), p(x + 2 * stepSize), p(x + 3 * stepSize),
                p(x + 4 * stepSize), p(x + 5 * stepSize), p(x + 6 * stepSize),
                p(x + 7 * stepSize), p(x + 8 * stepSize)
            };

            constexpr auto expected = p.derivative<DerivativeOrder>(x);
            constexpr auto derivative = Derivative<DerivativeOrder, Scheme, Error>();
            constexpr auto calculated = derivative.evaluate(p_x.data() + 8, stepSize);

            std::string message = errorName(Error);
            auto printerConfig = TestUtility::DoublePrinterConfig();
            printerConfig.precision = 12;            
            printerConfig.tolerance = 9;            

            auto result = TestUtility::compareDoubles(calculated, expected, message, printerConfig);
            if constexpr (StaticAssert)
                static_assert(result, "Derivative Test Failed");
        }

        std::string errorName(ErrorOrder error) const {
            switch (error) {
                case ErrorOrder::O_1: return "O(1)";
                case ErrorOrder::O_2: return "O(2)";
                case ErrorOrder::O_3: return "O(3)";
                case ErrorOrder::O_4: return "O(4)";
                case ErrorOrder::O_5: return "O(5)";
                case ErrorOrder::O_6: return "O(6)";
                default: return "Unknown";
            }
        }

    };


} // namespace STLKR_Tests


#endif // DIFFERENTIATION_VALIDITYTESTS_H