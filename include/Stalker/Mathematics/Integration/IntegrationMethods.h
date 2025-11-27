#ifndef NUMERICALINTEGRATIONMETHODS_H
#define NUMERICALINTEGRATIONMETHODS_H

#include <Stalker/Mathematics/Vector/VectorMathMeta.h>
#include <numeric>

namespace Stalker::Mathematics {

enum class IntegrationType {
    Trapezoidal,
    Simpson1,
    Simpson2,
    GaussLegendre,
    GaussHermit,
    GaussChebyshev,
    None
};

template <IntegrationType Type, typename T>
struct IntegrationMethod;

template <typename T>
struct IntegrationMethod<IntegrationType::Trapezoidal, T> {
    
    static constexpr inline double integrate(const T *f_x, size_t size, double h) {
        if (size > 2) {
            double result = 0;
            for (size_t i = 1; i < size - 1; ++i)
                result += f_x[i];
            return 0.5 * h * (f_x[0] + 2 * result + f_x[size - 1]) ;
        }
        if (size < 2) return 0;
        if (size == 2)return 0.5 * h * (f_x[0] + f_x[1]);
    }

    template <size_t Size>
    static constexpr inline double integrate(const T *f_x, double h) {
        return _integrateUnrolled<>(f_x, h, std::make_index_sequence<Size>());
    }

    private:
        template <size_t... IndexSequence>
        static constexpr inline double _integrateUnrolled(const T *f_x, double h, std::index_sequence<IndexSequence...>) {
            constexpr auto size = sizeof...(IndexSequence);
            if constexpr (size < 2) return 0;
            else {
                constexpr auto middleSum = ((IndexSequence > 0 && IndexSequence < size - 1 ? f_x[IndexSequence] : 0.0) + ...);
                return 0.5 * h * (f_x[0] + 2 * middleSum + f_x[size - 1]);
            }
        }
};

template <typename T>
struct IntegrationMethod<IntegrationType::Simpson1, T> {
    static constexpr inline double integrate(const T *f_x, size_t size, double h) {
        if (size > 3) {
            double result = 0;
            for (size_t i = 1; i < size - 1; ++i){
                if (i % 2 == 0)
                    result += 2 * f_x[i];
                else
                    result += 4 * f_x[i];
            }
            return h / 3.0 * (f_x[0] + result + f_x[size - 1]);
        }
    }

    template <size_t Size>
    static constexpr inline double integrate(const T *f_x, double h) {
        if constexpr (Size < 3)
            return 0;
        else if constexpr (Size == 3)
            return h / 3.0 * (f_x[0] + 4 * f_x[1] + f_x[2]);
            
    }

    private:
        template <size_t... IndexSequence>
        static constexpr inline double _integrateUnrolled(const T *f_x, double h, std::index_sequence<IndexSequence...>) {
            constexpr auto size = sizeof...(IndexSequence);
            if constexpr (size < 3) return 0;
            else {
                constexpr auto middleSumOdd =  ((IndexSequence > 0 && IndexSequence < size - 1 && IndexSequence % 2 != 0 ? 4 * f_x[IndexSequence] : 0.0) + ...);
                constexpr auto middleSumEven = ((IndexSequence > 0 && IndexSequence < size - 1 && IndexSequence % 2 == 0 ? 2 * f_x[IndexSequence] : 0.0) + ...);
                return h / 3.0 * (f_x[0] + middleSumEven + middleSumOdd + f_x[size - 1]);
            }
        }
};

template <typename T>
struct IntegrationMethod<IntegrationType::Simpson2, T> {
    static constexpr inline double integrate(const T *f_x, size_t size, double h) {
        if (size < 3)
            return 0;
        else if (size == 3)
            return 3.0 / 8.0 * h * (f_x[0] + 3 * f_x[1] + f_x[2]);
        else {
            double result = 0;
            for (size_t i = 1; i < size - 1; ++i)
                result += f_x[i];
            return 3.0 / 8.0 * h * (f_x[0] + 3 * result + f_x[size - 1]) ;
        }
    }

    template <size_t Size>
    static constexpr inline double integrate(const T *f_x, double h) {
        if constexpr (Size < 3)
            return 0;
        else if constexpr (Size == 3)
            return 3.0 / 8.0 * h * (f_x[0] + 3 * f_x[1] + f_x[2]);
        else
            return 3.0 / 8.0 * h * (f_x[0] + 3 * VectorMathMeta::sum<T, Size - 2>(Size - 2, f_x) + f_x[Size - 1]) ;
    }
};

} // namespace Stalker::Mathematics
#endif // NUMERICALINTEGRATIONMETHODS_H