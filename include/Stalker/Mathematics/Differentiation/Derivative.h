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

#include <Stalker/Mathematics/Differentiation/FiniteDifferenceScheme.h>
namespace Stalker::Mathematics::Differentiation {

using DerivativeOrder = unsigned int;

template <DerivativeOrder Order, T_Scheme Scheme, ErrorOrder Error>
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

} // namespace Stalker::Mathematics::Differentiation 