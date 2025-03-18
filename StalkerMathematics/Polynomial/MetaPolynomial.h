#ifndef TPOLYNOMIAL_H
#define TPOLYNOMIAL_H

#include <array>
#include <vector>
#include <cstddef>
#include "../MetaMath.h"
#include "../Utility/Linspace.h"

template <typename T, size_t Order>
class  MetaPolynomial {
public:

    constexpr MetaPolynomial(const std::array<T, Order + 1>& coefficients) 
        : _coefficients(coefficients) {}

    constexpr double operator()(T x) const {
        return _evaluateHorner<Order>(x);
    }

    constexpr double evaluateNaive(T x) const {
        return _evaluateNaive<Order>(x);
    }

    constexpr double evaluateHorner(T x) const {
        return _evaluateHorner<Order>(x);
    }

    template <size_t NPoints>
    constexpr std::array<double, NPoints> sample(T start, T end) const {
        return _sample(start, end, std::make_index_sequence<NPoints>());
    }

    std::vector<double> sample(T start, T end, size_t numPoints) const {
        auto x = linspace(start, end, numPoints);
        std::vector<double> y(numPoints);
        for (size_t i = 0; i < numPoints; ++i) {
            y[i] = operator()(x[i]);
        }
        return y;
    }

    template <size_t IDerivativeOrder>
    constexpr double derivative(T x) const {
        return _evaluateDerivative<IDerivativeOrder, Order>(x);
    }

    constexpr double integral(double x1, double x2) const {
        return _evaluateIntegral<Order>(x2) - _evaluateIntegral<Order>(x1);
    }

private:
    std::array<T, Order + 1> _coefficients;  ///< Coefficients of the polynomial.

    template <size_t IOrder>  
    constexpr T _evaluateHorner(T x) const {
        if constexpr (IOrder == 0)
            return _coefficients[Order];
        else
            return _evaluateHorner<IOrder - 1>(x) * x + _coefficients[Order - IOrder];
    }

    template <size_t IOrder>
    constexpr T _evaluateNaive(T x) const {
        if constexpr (IOrder <= Order)
            return _coefficients[IOrder] * power<IOrder>(x) + _evaluateNaive<IOrder - 1>(x);
        else
            return 0;
    }
    
    template <size_t K, size_t CurrentOrder>
    constexpr T _evaluateDerivative(T x) const {
        if constexpr (CurrentOrder < K) return 0;  // Term doesn't contribute
        else {
            return _coefficients[CurrentOrder] * fallingFactorial<CurrentOrder, K>() * power<CurrentOrder - K>(x)
            + _evaluateDerivative<K, CurrentOrder - 1>(x);
        }
    }

    template <size_t IOrder>
    constexpr T _evaluateIntegral(T x) const {
        if constexpr (IOrder == 0)
            return _coefficients[0] * x;
        else
            return _coefficients[IOrder] * power<IOrder + 1>(x) / (IOrder + 1)
                + _evaluateIntegral<IOrder - 1>(x);
    }
    template<size_t... IndexSequence>
    constexpr auto _sample(T start, T end, std::index_sequence<IndexSequence...>) const {
        auto x = linspace<sizeof...(IndexSequence)>(start, end);
        return std::array<double, sizeof...(IndexSequence)>{{ operator()(x[IndexSequence])... }};
    } 

};

#endif // TPOLYNOMIAL_H
