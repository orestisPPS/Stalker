#pragma once

#include <cmath>

namespace Stalker::Core{

enum class ErrorType {
    Absolute,
    Relative,
    Percentage
};
template<typename T, typename ResultT = double>
class Error {
public:

    ResultT operator()(T expected, T actual, ErrorType type = ErrorType::Percentage) const {
        switch (type) {
            case ErrorType::Absolute:
                return absoluteError(expected, actual);
            case ErrorType::Relative:
                return relativeError(expected, actual);
            default:
                return percentageError(expected, actual);
        }
    }

    static constexpr inline ResultT
    absoluteError(T expected, T actual) {
        return std::abs(expected - actual);
    }

    static constexpr inline ResultT
    relativeError(T expected, T actual){
        return absoluteError(expected, actual) / expected;
    };

    static constexpr inline ResultT
    percentageError(T expected, T actual) {
        return relativeError(expected, actual) * 100;
    }
};
}
