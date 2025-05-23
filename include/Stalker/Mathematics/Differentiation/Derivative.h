#ifndef STALKER_MATHEMATICS_DERIVATIVE_CALCULATOR_H
#define STALKER_MATHEMATICS_DERIVATIVE_CALCULATOR_H

#include "FiniteDifferenceScheme.h"
#include "../MetaMath.h"


using DerivativeOrder = unsigned int;

template <DerivativeOrder Order, SchemeType Scheme, ErrorOrder Error>
class Derivative {

public:

    constexpr double operator()(const double *x, double h) {
        return evaluate(x, h);
    }
    
    static constexpr double evaluate(const double *x, double h) {
        constexpr auto scheme = FiniteDifferenceScheme<Order, Scheme, Error>();
        return _evaluateSum(x, std::make_index_sequence<scheme.nPoints()>{}) / scheme.denominator(h);
    }

    static constexpr auto weights() {
        return FiniteDifferenceScheme<Order, Scheme, Error>().weights();
    }

    static constexpr auto stencil() {
        return FiniteDifferenceScheme<Order, Scheme, Error>().stencil();
    }

    static constexpr auto denominator(double h) {
        return FiniteDifferenceScheme<Order, Scheme, Error>().denominator(h);
    }

private:
    template<size_t... IndexSequence>
    static constexpr double _evaluateSum(const double *x, std::index_sequence<IndexSequence...>) {
        constexpr auto scheme = FiniteDifferenceScheme<Order, Scheme, Error>();
        return ((scheme.weights()[IndexSequence] * x[scheme.stencil()[IndexSequence]]) + ...);
    }
};

#endif // STALKER_MATHEMATICS_DERIVATIVE_CALCULATOR_H