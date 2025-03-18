#ifndef STALKER_MATHEMATICS_ERROR_H
#define STALKER_MATHEMATICS_ERROR_H

template<typename T>
class Error {
public:

    static constexpr inline T
    absoluteError(T expected, T actual) {
        return std::abs(expected - actual);
    }

    static constexpr inline T
    relativeError(T expected, T actual)
    { return absoluteError(expected, actual) / expected; };


    static constexpr inline T percentageError(T expected, T actual) {
        return relativeError(expected, actual) * 100;
    }
};

#endif // STALKER_MATHEMATICS_ERROR_H