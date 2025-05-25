#pragma once
#include <cmath>

namespace Stalker::Core {

enum class ErrorType {
    Absolute,
    Relative,
    Percent
};

template<typename T, typename ResultT = double>
constexpr ResultT errorAbsolute(T expected, T actual) {
    return std::abs(expected - actual);
}

template<typename T, typename ResultT = double>
constexpr ResultT errorRelative(T expected, T actual) {
    return errorAbsolute<T, ResultT>(expected, actual) / expected;
}

template<typename T, typename ResultT = double>
constexpr ResultT errorPercent(T expected, T actual) {
    return errorRelative<T, ResultT>(expected, actual) * 100;
}

template<typename T, typename ResultT = double>
constexpr ResultT error(T expected, T actual, ErrorType type = ErrorType::Percent) {
    switch(type) {
        case ErrorType::Absolute: return errorAbsolute<T, ResultT>(expected, actual);
        case ErrorType::Relative: return errorRelative<T, ResultT>(expected, actual);
        case ErrorType::Percent:
        default: return errorPercent<T, ResultT>(expected, actual);
    }
}

} // namespace Stalker::Core
