#ifndef NUMERIC_TYPE_CONSTRAINTS_H
#define NUMERIC_TYPE_CONSTRAINTS_H

#include <type_traits>
#include <limits>
#include <concepts>

template <typename T>
concept SupportedNumericalType = std::same_as<T, int>   || std::same_as<T, short>  || 
                                 std::same_as<T, float> || std::same_as<T, double> || 
                                 std::same_as<T, unsigned>;

template <typename T>
concept SupportedNumericalOperations = SupportedNumericalType<T> && requires(T a, T b) {
    { a + b } -> std::same_as<T>;  // Closure under addition
    { a - b } -> std::same_as<T>;  // Closure under subtraction
    { a * b } -> std::same_as<T>;  // Closure under multiplication
    { a / b } -> std::same_as<T>;  // Closure under division
};

template <typename T>
concept TotallyOrdered = SupportedNumericalType<T> && requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;  // Equality
    { a != b } -> std::convertible_to<bool>;  // Inequality
    { a < b } -> std::convertible_to<bool>;   // Less than
    { a <= b } -> std::convertible_to<bool>;  // Less than or equal
    { a > b } -> std::convertible_to<bool>;   // Greater than
    { a >= b } -> std::convertible_to<bool>;  // Greater than or equal
};

template <typename T>
concept BoundedNumerical = SupportedNumericalType<T> && requires {
    { std::numeric_limits<T>::min() } -> std::convertible_to<T>;  // Lower bound
    { std::numeric_limits<T>::max() } -> std::convertible_to<T>;  // Upper bound
};

template <typename T>
concept Numerical = SupportedNumericalOperations<T> && TotallyOrdered<T> && BoundedNumerical<T>;

#endif // NUMERIC_TYPE_CONSTRAINTS_H