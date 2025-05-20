#ifndef NUMERICAL_CONCEPTS_H
#define NUMERICAL_CONCEPTS_H

#include <concepts>
#include <type_traits>
#include <cmath>
#include <limits>
#include <numbers>


// Fundamental algebraic concepts
template<typename T>
concept Additive = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
    { a - b } -> std::convertible_to<T>;
    { -a }    -> std::convertible_to<T>;
    { T(0) }  -> std::same_as<T>;
};

template<typename T>
concept Multiplicative = requires(T a, T b) {
    { a * b } -> std::convertible_to<T>;
    { a / b } -> std::convertible_to<T>;
    { T(1) }  -> std::same_as<T>;
};

template<typename T>
concept Ordered = requires(T a, T b) {
    { a < b }  -> std::convertible_to<bool>;
    { a > b }  -> std::convertible_to<bool>;
    { a <= b } -> std::convertible_to<bool>;
    { a >= b } -> std::convertible_to<bool>;
};

template<typename T>
concept EqualityComparable = requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a != b } -> std::convertible_to<bool>;
};

template<typename T>
concept Bitwise = requires(T a, T b) {
    { a | b } -> std::same_as<T>;
    { a & b } -> std::same_as<T>;
    { a ^ b } -> std::same_as<T>;
    { ~a }    -> std::same_as<T>;
    { a << b }-> std::same_as<T>;
    { a >> b }-> std::same_as<T>;
};

// Core numeric concept
template<typename T>
concept Numeric =
    std::is_arithmetic_v<T> &&
    Additive<T> &&
    Multiplicative<T> &&
    Ordered<T> &&
    EqualityComparable<T> &&
    !std::is_same_v<std::remove_cv_t<T>, bool> &&
    !std::is_same_v<std::remove_cv_t<T>, char> &&
    !std::is_same_v<std::remove_cv_t<T>, signed char> &&
    !std::is_same_v<std::remove_cv_t<T>, unsigned char> &&
    !std::is_same_v<std::remove_cv_t<T>, wchar_t> &&
    !std::is_same_v<std::remove_cv_t<T>, char16_t> &&
    !std::is_same_v<std::remove_cv_t<T>, char32_t> &&
    requires {
        typename std::numeric_limits<T>;
        requires std::numeric_limits<T>::is_specialized;
        requires std::numeric_limits<T>::lowest() < std::numeric_limits<T>::max();
    };

// Floating point refinement
template<typename T>
concept FloatingPoint =
    Numeric<T> &&
    std::is_floating_point_v<T> &&
    requires(T a, T b) {
        { std::abs(a) }   -> std::same_as<T>;
        { std::sqrt(a) }  -> std::same_as<T>;
        { std::floor(a) } -> std::same_as<T>;
        { std::ceil(a) }  -> std::same_as<T>;
        { std::trunc(a) } -> std::same_as<T>;
        { std::round(a) } -> std::same_as<T>;
        { std::fmod(a, b) } -> std::same_as<T>;
    };

// Integer refinement
template<typename T>
concept Integer =
    Numeric<T> &&
    std::is_integral_v<T> &&
    Bitwise<T> &&
    requires(T a, T b) {
        { a % b } -> std::same_as<T>;
        { a / b } -> std::same_as<T>;  // Truncating division
    } &&
    requires {
        requires std::numeric_limits<T>::is_integer;
        requires !std::numeric_limits<T>::is_modulo || 
                 std::numeric_limits<T>::is_signed;
    };

// Sign concepts
template<typename T>
concept Signed = Numeric<T> && std::is_signed_v<T>;

template<typename T>
concept Unsigned = Numeric<T> && std::is_unsigned_v<T>;

// Integer specializations
template<typename T>
concept SignedInteger = Integer<T> && Signed<T>;

template<typename T>
concept UnsignedInteger = Integer<T> && Unsigned<T>;

// Natural numbers (non-negative integers)
template<typename T>
concept Natural =
    UnsignedInteger<T> &&
    requires(T a) {
        { T(0) } -> std::same_as<T>;
        { a + T(1) } -> std::same_as<T>;
    } &&
    requires {
        requires std::numeric_limits<T>::min() == 0;
        requires std::numeric_limits<T>::max() > 0;
    };

// Real numbers (union of integers and floats)
template<typename T>
concept Real = FloatingPoint<T> || Integer<T>;


template<typename T>
concept AlignedNumeric = 
  Numeric<T> &&
  requires {
    requires ((alignof(T) & (alignof(T) - 1)) == 0);  // Power-of-two alignment
    requires ((sizeof(T) & (sizeof(T) - 1)) == 0);    // Power-of-two size

    requires (sizeof(T) <= alignof(T)) && 
             (alignof(T) % sizeof(T) == 0);
  } &&
  requires(T* ptr) {
    { std::assume_aligned<alignof(T)>(ptr) } -> std::same_as<T*>;
  };







#endif // NUMERICAL_CONCEPTS_H