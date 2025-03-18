#ifndef INTEGRAL_H
#define INTEGRAL_H

#include "IntegrationMethods.h"

template <IntegrationType Type, typename T>
class Integral {
public:

    double operator()(const T *f_x, size_t size, double h) const {
        return evaluate(f_x, size, h);
    }

    template <size_t Size>
    double operator()(const T *f_x, double h) const {
        return evaluate<Size>(f_x, h);
    }

    static constexpr double evaluate(const T *f_x, size_t size, double h) {
        return IntegrationMethod<Type, T>::integrate(f_x, size, h);
    }

    template <size_t Size>
    static constexpr double evaluate(const T *f_x, double h) {
        return IntegrationMethod<Type, T>::template integrate<Size>(f_x, h);
    }



};

#endif // INTEGRAL_H