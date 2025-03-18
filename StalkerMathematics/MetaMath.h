#ifndef META_MATH_H
#define META_MATH_H

#include <array>
#include <cstddef>
#include <type_traits>


static constexpr double pi() { return std::acos(-1); }

template<size_t Power>
static inline constexpr auto power(auto base) {
    if constexpr (Power == 0)
        return 1;
    else if constexpr (Power == 1) 
        return base;
    else if constexpr (Power % 2 == 0)
        return power<Power / 2>(base * base);
    else
        return base * power<Power / 2>(base * base);
}

// Optimized Factorial (Pure Compile-Time)
template<size_t N>
static inline constexpr size_t factorial() {
    if constexpr (N == 0)
        return 1;
    else
        return N * factorial<N - 1>();
}

template<size_t N, size_t K>
static inline constexpr size_t fallingFactorial() {
    if constexpr (K == 0)
        return 1;
    else if constexpr (N >= K)
        return N * fallingFactorial<N - 1, K - 1>();
    else
        return 0;
}

template<size_t N, size_t K>
static inline constexpr size_t binomialCoefficient() {
    if constexpr (K == 0 || K == N)
        return 1;
    else
        return (N + 1 - K) / K * binomialCoefficient<N, K - 1>();
}

// Optimized Fibonacci (Memoization)
template<size_t N>
struct Fibonacci {
    static constexpr int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};
template<> struct Fibonacci<0> { static constexpr int value = 0; };
template<> struct Fibonacci<1> { static constexpr int value = 1; };






#endif // META_MATH_H