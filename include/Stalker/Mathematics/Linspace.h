#ifndef LINSPACE_H
#define LINSPACE_H

#include <vector>
#include <stdexcept>
#include <utility> // for std::pair
#include <array>

template <typename T = double, typename StartType, typename EndType>
static inline void linspace(T* result, StartType start, EndType stop, int num = 50, bool endpoint = true) {
    if (num < 0)
        throw std::invalid_argument("linspace: number of samples must be non-negative");

    const T startValue = static_cast<T>(start);
    const T endValue  = static_cast<T>(stop);

    if (num == 0)
        return;

    if (num == 1) {
        result[0] = startValue;
        return;
    }

    const int denom = endpoint ? (num - 1) : num;
    if (denom <= 0)
        throw std::invalid_argument("linspace: invalid number of samples");

    const T step = (endValue - startValue) / denom;
    for (int i = 0; i < num; ++i)
        result[i] = startValue + i * step;
}

template <typename T = double, typename StartType, typename EndType>
static inline void linspace(std::vector<double>& result, StartType start, EndType stop, int num = 50, bool endpoint = true) {
    if (num < 0)
        throw std::invalid_argument("linspace: number of samples must be non-negative");

    result.resize(num);
    linspace(result.data(), start, stop, num, endpoint);
}

template <typename T = double, typename StartType, typename EndType>
static inline std::vector<double> linspace(StartType start, EndType stop, int num = 50, bool endpoint = true) {
    if (num < 0)
        throw std::invalid_argument("linspace: invalid return size");

    std::vector<double> result(num);
    linspace(result.data(), start, stop, num, endpoint);
    return result;
}

template <size_t Num = 50, typename StartType, typename EndType>
constexpr std::array<double, Num> linspace(StartType start, EndType end, bool endpoint = true) {
    static_assert(Num > 0, "N must be greater than 0");

    return []<size_t... IndexSequence>(StartType s, EndType e, bool ep, std::index_sequence<IndexSequence...>) {
        std::array<double, Num> result{};
        if constexpr (Num == 1) {
            result[0] = s;
        } else {
            const double denom = ep ? (Num - 1) : Num;
            const double step  = (e - s) / denom;
            ((result[IndexSequence] = s + IndexSequence * step), ...);
        }
        return result;
    }
    (start, end, endpoint, std::make_index_sequence<Num>{});
}

#endif // LINSPACE_H